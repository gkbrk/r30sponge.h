from ctypes import *

class Rule30Sponge:
    def __init__(self):
        self._lib = CDLL("./r30sponge.so")
        self._sp = self.new_default()

    def absorb_byte(self, byte):
        f = self._lib.r30s_absorb_byte
        f.argtypes = [c_void_p, c_ubyte]
        f.restype = None
        f(self._sp, c_ubyte(byte))

    def squeeze_bit(self):
        f = self._lib.r30s_squeeze_bit
        f.argtypes = [c_void_p]
        f.restype = c_ubyte
        return f(self._sp)

    def squeeze_byte(self):
        f = self._lib.r30s_squeeze_byte
        f.argtypes = [c_void_p]
        f.restype = c_ubyte
        return f(self._sp)

    @property
    def new_default(self):
        f = self._lib.r30s_new_default
        f.argtypes = []
        f.restype = c_void_p
        return f
