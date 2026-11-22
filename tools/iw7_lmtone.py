# v129: fold IW7's tone curve into the H1 secondary lightmap. IW7 draws its HDR light through the map's vision tone map
# (vision/mp_flip.vision: auto exposure, white 512, crossover 2, shoulder 0.95, toe 0.2; curve from fastfile_iw7 light.rs),
# H1's Ghosts-style world shaders draw it nearly linear, so bright spots blew out. For a typical albedo a0 the light is
# remapped so that a0 * L' = T(a0 * L * E) * gain, applied to both pages by one factor so the hue and direction stay.
# usage: python3 iw7_lmtone.py <sec in> <sec out> [exposure 6.15] [gain 1.5]
import sys, numpy as np
def T(x, b=0.0, w=512.0, c=2.0, s=0.95, t=0.2):
    k = (1 - t) * (c - b) / ((1 - s) * (w - c) + (1 - t) * (c - b))
    x = np.maximum(x, 0)
    return np.clip(np.where(x < c, k * (1 - t) * (x - b) / (c - (1 - t) * b - t * x), k + (1 - k) * (x - c) / (s * x + (1 - s) * w - c)), 0, 1)
E = float(sys.argv[3]) if len(sys.argv) > 3 else 6.15
G = float(sys.argv[4]) if len(sys.argv) > 4 else 1.5
d = open(sys.argv[1], 'rb').read(); hdr = len(d) - 4096 * 1024 * 8
s = np.frombuffer(d[hdr:], np.float16).reshape(4096, 1024, 4).astype(np.float32)
lum = lambda a: a @ np.array([0.2126, 0.7152, 0.0722], np.float32)
C = lum(s[:2048, :, :3]) + 0.8 * lum(s[2048:, :, :3]); lit = C > 1e-3
f = np.where(lit, T(0.35 * C * E) / 0.35 * G / np.maximum(C, 1e-6), 1.0)
s[:2048, :, :3] *= f[..., None]; s[2048:, :, :3] *= f[..., None]
open(sys.argv[2], 'wb').write(d[:hdr] + s.astype(np.float16).tobytes())
