# v128: put IW7's baked light direction (the *_secondunorm map, R8G8 turned 45 degrees: r = R*2-1, g = G*2-1,
# x = (r+g)/2, y = (r-g)/2, z = 1-|x|-|y|) into the H1 secondary's alphas, which the Ghosts-style world
# shaders read as the directional page's tangent-space direction (upper page .a = x, lower page .a = y).
# usage: python3 iw7_lmdir.py <iw7 secondunorm.h1Image> <h1 secondary in> <h1 secondary out>
import sys, numpy as np
su = open(sys.argv[1], 'rb').read()
a = np.frombuffer(su[len(su) - (1 << 24):], np.uint8).reshape(4096, 2048, 2).astype(np.float32) / 255
r = a[..., 0] * 2 - 1; g = a[..., 1] * 2 - 1
x = (r + g) / 2; y = (r - g) / 2; z = np.maximum(1 - np.abs(x) - np.abs(y), 0)
v = np.stack([x, y, z], -1); v /= np.maximum(np.linalg.norm(v, axis=-1, keepdims=True), 1e-6)
v = v.reshape(2048, 2, 1024, 2, 3).mean((1, 3)); v /= np.maximum(np.linalg.norm(v, axis=-1, keepdims=True), 1e-6)
d = open(sys.argv[2], 'rb').read(); hdr = len(d) - 4096 * 1024 * 8
s = np.frombuffer(d[hdr:], np.float16).reshape(4096, 1024, 4).copy()
s[:2048, :, 3] = v[..., 0]; s[2048:, :, 3] = v[..., 1]
open(sys.argv[3], 'wb').write(d[:hdr] + s.tobytes())
