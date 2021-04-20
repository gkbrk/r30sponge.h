import r30s

sp = r30s.Rule30Sponge()

for b in "Hello world!":
    b = ord(b)
    sp.absorb_byte(b)

assert sp.squeeze_byte() == 231
assert sp.squeeze_byte() == 190
assert sp.squeeze_byte() == 6
assert sp.squeeze_byte() == 124
assert sp.squeeze_byte() == 69
