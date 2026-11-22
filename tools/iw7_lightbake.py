# v121 lightmap baker: adds IW7's real-time (forward+) primary lights into the H1 secondary lightmap.
# usage: python3 bake.py <v121_bake.bin> <v121_lights.bin> <sec_in.h1Image> <sec_out.h1Image> [gain]
import sys, struct, math, time
import numpy as np
from numba import njit, prange

W, H = 1024, 2048           # one secondary layer (two stacked: top, bottom ~3.6x brighter)
VOX = 8.0                   # occupancy voxel size (units) for shadow rays
RATIO = 3.6                 # bottom/top layer ratio measured on the current secondary
USE_VIS = False

def load_tris(path):
    d = open(path, 'rb').read()
    assert d[:4] == b'BK21'
    n = struct.unpack_from('<I', d, 4)[0]
    rec = np.dtype([('caster', '<u4'), ('v', [('xyz', '<f4', 3), ('n', '<u4'), ('uv', '<f4', 2)], 3)])
    a = np.frombuffer(d, rec, n, 8)
    return a

def load_lights(path):
    d = open(path, 'rb').read()
    assert d[:4] == b'LT21'
    n = struct.unpack_from('<I', d, 4)[0]
    out = []
    o = 8
    for i in range(n):
        f = struct.unpack_from('<I3f3f3f8f', d, o); o += 4 + 4 * 17
        t = f[0]; col = f[1:4]; dr = f[4:7]; org = f[7:10]
        radius, cosO, cosI, ex, dfall, uvI, irI, bulb = f[10:18]
        out.append(dict(type=t, color=col, dir=dr, origin=org, radius=radius, cosO=cosO, cosI=cosI, exp=ex, dfall=dfall, uv=uvI, ir=irI, bulb=bulb))
    return out

def decode_normals(packed, mode):
    p = packed.astype(np.uint64)
    if mode == 0:   # 10:10:10:2 unsigned, (v/1023)*2-1
        x = (p & 1023) / 1023.0 * 2 - 1; y = ((p >> 10) & 1023) / 1023.0 * 2 - 1; z = ((p >> 20) & 1023) / 1023.0 * 2 - 1
    elif mode == 1: # 10:10:10 signed two's complement /511
        def s(v):
            v = v.astype(np.int64); return np.where(v >= 512, v - 1024, v) / 511.0
        x = s(p & 1023); y = s((p >> 10) & 1023); z = s((p >> 20) & 1023)
    else:           # bytes, (b-127.5)/127.5 (old IW)
        x = ((p & 255) - 127.5) / 127.5; y = (((p >> 8) & 255) - 127.5) / 127.5; z = (((p >> 16) & 255) - 127.5) / 127.5
    n = np.stack([x, y, z], -1).astype(np.float32)
    n /= np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-6)
    return n

@njit(cache=True)
def voxelize(P, caster, mn, dims, vox):
    occ = np.zeros((dims[0], dims[1], dims[2]), np.uint8)
    for t in range(P.shape[0]):
        if caster[t] == 0:
            continue
        a = P[t, 0]; b = P[t, 1]; c = P[t, 2]
        cx = (a[0] + b[0] + c[0]) / 3.0; cy = (a[1] + b[1] + c[1]) / 3.0
        if cx < mn[0] - 512 or cy < mn[1] - 512 or cx > mn[0] + dims[0] * vox + 512 or cy > mn[1] + dims[1] * vox + 512:
            continue
        e = max(np.sqrt(((b - a) ** 2).sum()), np.sqrt(((c - a) ** 2).sum()), np.sqrt(((c - b) ** 2).sum()))
        k = int(e / (vox * 0.5)) + 1
        for i in range(k + 1):
            for j in range(k + 1 - i):
                u = i / k; v = j / k
                p = a + (b - a) * u + (c - a) * v
                ix = int((p[0] - mn[0]) / vox); iy = int((p[1] - mn[1]) / vox); iz = int((p[2] - mn[2]) / vox)
                if 0 <= ix < dims[0] and 0 <= iy < dims[1] and 0 <= iz < dims[2]:
                    occ[ix, iy, iz] = 1
    return occ

@njit(cache=True)
def visible(occ, mn, dims, vox, p, l):
    d = l - p
    dist = np.sqrt((d * d).sum())
    if dist < 1e-3:
        return 1.0
    d = d / dist
    s = vox * 2.0          # skip the receiver's own voxels
    step = vox * 0.5
    while s < dist - vox * 1.5:
        q = p + d * s
        ix = int((q[0] - mn[0]) / vox); iy = int((q[1] - mn[1]) / vox); iz = int((q[2] - mn[2]) / vox)
        if 0 <= ix < dims[0] and 0 <= iy < dims[1] and 0 <= iz < dims[2]:
            if occ[ix, iy, iz]:
                return 0.0
        s += step
    return 1.0

@njit(cache=True)
def shade(p, n, L, occ, mn, dims, vox):
    # L rows: type, ox,oy,oz, dx,dy,dz, r,g,b, radius, cosO, cosI, bulb
    acc = np.zeros(3, np.float32)
    for i in range(L.shape[0]):
        lo = L[i, 1:4]
        d = lo - p
        dist = np.sqrt((d * d).sum())
        rad = L[i, 10]
        if dist >= rad or dist < 1e-3:
            continue
        ld = d / dist
        ndl = n[0] * ld[0] + n[1] * ld[1] + n[2] * ld[2]
        if ndl <= 0.0:
            continue
        x = dist / rad
        win = (1.0 - x * x)
        win = win * win
        bulb = max(L[i, 13], 8.0)
        att = win * (bulb * bulb) / (dist * dist + bulb * bulb)
        if L[i, 0] == 2.0:   # spot
            cd = (ld[0] * L[i, 4] + ld[1] * L[i, 5] + ld[2] * L[i, 6])   # IW7 dir points toward the light (like the sun); spots shine along -dir
            co = L[i, 11]; ci = L[i, 12]
            if ci <= co:
                ci = co + 1e-3
            tt = (cd - co) / (ci - co)
            if tt <= 0.0:
                continue
            if tt > 1.0:
                tt = 1.0
            att *= tt * tt * (3.0 - 2.0 * tt)
        if att * ndl < 1e-4:
            continue
        v = visible(occ, mn, dims, vox, p + n * 2.0, lo) if USE_VIS else 1.0
        if v == 0.0:
            continue
        w = att * ndl
        acc[0] += L[i, 7] * w; acc[1] += L[i, 8] * w; acc[2] += L[i, 9] * w
    return acc

@njit(parallel=True, cache=True)
def bake(P, N, UV, L, occ, mn, dims, vox, out, cnt):
    T = P.shape[0]
    for t in prange(T):
        u0 = UV[t, 0, 0] * W; v0 = UV[t, 0, 1] * H
        u1 = UV[t, 1, 0] * W; v1 = UV[t, 1, 1] * H
        u2 = UV[t, 2, 0] * W; v2 = UV[t, 2, 1] * H
        x0 = int(max(min(u0, u1, u2) - 0.5, 0)); x1 = int(min(max(u0, u1, u2) + 0.5, W - 1))
        y0 = int(max(min(v0, v1, v2) - 0.5, 0)); y1 = int(min(max(v0, v1, v2) + 0.5, H - 1))
        den = (v1 - v2) * (u0 - u2) + (u2 - u1) * (v0 - v2)
        if abs(den) < 1e-12:
            continue
        for yy in range(y0, y1 + 1):
            for xx in range(x0, x1 + 1):
                px = xx + 0.5; py = yy + 0.5
                a = ((v1 - v2) * (px - u2) + (u2 - u1) * (py - v2)) / den
                b = ((v2 - v0) * (px - u2) + (u0 - u2) * (py - v2)) / den
                c = 1.0 - a - b
                if a < -0.05 or b < -0.05 or c < -0.05:   # small dilation into the gutter
                    continue
                p = P[t, 0] * a + P[t, 1] * b + P[t, 2] * c
                n = N[t, 0] * a + N[t, 1] * b + N[t, 2] * c
                nl = np.sqrt((n * n).sum())
                if nl < 1e-6:
                    continue
                n = n / nl
                e = shade(p, n, L, occ, mn, dims, vox)
                out[yy, xx, 0] += e[0]; out[yy, xx, 1] += e[1]; out[yy, xx, 2] += e[2]
                cnt[yy, xx] += 1.0

def main():
    tris_p, lights_p, sec_in, sec_out = sys.argv[1:5]
    gain = float(sys.argv[5]) if len(sys.argv) > 5 else 1.0
    t0 = time.time()
    tr = load_tris(tris_p)
    lights = load_lights(lights_p)
    print('tris', len(tr), 'lights', len(lights))
    P = np.ascontiguousarray(tr['v']['xyz'].astype(np.float32))
    UV = np.ascontiguousarray(tr['v']['uv'].astype(np.float32))
    caster = np.ascontiguousarray(tr['caster'].astype(np.uint8))
    face = np.cross(P[:, 1] - P[:, 0], P[:, 2] - P[:, 0])
    face /= np.maximum(np.linalg.norm(face, axis=-1, keepdims=True), 1e-6)
    best = None
    for mode in range(3):
        Nm = decode_normals(tr['v']['n'], mode)
        agree = np.abs((Nm.mean(1) * face).sum(-1)).mean()
        print('normal mode', mode, 'agreement', round(float(agree), 3))
        if best is None or agree > best[0]:
            best = (agree, mode, Nm)
    N = np.ascontiguousarray(best[2])
    # orient vertex normals with the face normal sign
    s = np.sign((N.mean(1) * face).sum(-1))
    print('normal flip fraction', float((s < 0).mean()))
    types = {}
    for l in lights: types[l['type']] = types.get(l['type'], 0) + 1
    print('light types', types)
    rows = []
    for l in lights:
        if l['type'] not in (2, 3):
            continue
        c = np.array(l['color'], np.float32)
        if c.max() <= 0 or l['radius'] <= 0:
            continue
        rows.append([float(l['type']), *l['origin'], *l['dir'], *c, l['radius'], l['cosO'], l['cosI'], l['bulb']])
    L = np.array(rows, np.float32)
    print('baked lights', len(L), 'color p50/p99', np.percentile(L[:, 7:10].max(1), [50, 99]) if len(L) else None,
          'radius p50', np.percentile(L[:, 10], 50) if len(L) else None)
    # grid only over the region the lights reach (the far vista city made it 120k units wide)
    R = L[:, 10:11]
    mn = (L[:, 1:4] - R).min(0) - VOX * 2
    mx = (L[:, 1:4] + R).max(0) + VOX * 2
    dims = np.ceil((mx - mn) / VOX).astype(np.int64)
    print('grid', dims, 'cells', int(np.prod(dims)))
    occ = voxelize(P, caster, mn.astype(np.float32), dims, VOX)
    print('occupied', float(occ.mean()), 'voxelize s', round(time.time() - t0, 1))
    out = np.zeros((H, W, 3), np.float32); cnt = np.zeros((H, W), np.float32)
    bake(P, N, UV, L, occ, mn.astype(np.float32), dims, VOX, out, cnt)
    E = out / np.maximum(cnt, 1)[..., None]
    lit0 = E.max(-1) > 0
    if gain <= 0:   # auto: lit texels' 90th percentile -> 0.7 (top layer units; current secondary p50 0.58, p90 1.0)
        gain = 1.2 / max(np.percentile(E.max(-1)[lit0], 50), 1e-9)   # lit texels' median -> 1.2 (bright plaza ~2)
        print('auto gain', gain)
    E = E * gain
    np.save('/tmp/bake/E.npy', E)
    lit = E.max(-1) > 1e-3
    print('bake s', round(time.time() - t0, 1), 'texels lit', int(lit.sum()), 'E p50/p90/p99', np.percentile(E.max(-1)[lit], [50, 90, 99]) if lit.any() else None)
    d = open(sec_in, 'rb').read()
    a = np.frombuffer(d[139:], np.float16).reshape(4096, 1024, 4).astype(np.float32).copy()
    for k, f in ((0, 1.0), (1, RATIO)):
        lay = a[k * H:(k + 1) * H, :, :3]
        cap = 2.4 * f
        add = E * f
        lay2 = lay + add
        # soft shoulder above the current maximum range so lamps don't blow out
        Ls = lay2.max(-1, keepdims=True)
        sc = np.where(Ls > cap, cap * (1 + np.log(Ls / cap)) / np.maximum(Ls, 1e-6), 1.0)
        a[k * H:(k + 1) * H, :, :3] = lay2 * sc
    open(sec_out, 'wb').write(d[:139] + a.astype(np.float16).tobytes())
    print('wrote', sec_out, 'total s', round(time.time() - t0, 1))

if __name__ == '__main__':
    main()
