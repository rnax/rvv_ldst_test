#define dst a0
#define src a1
#define in_vstart a2
#define ivl a3

#define LMUL m8
#define vdata v0

.global memcpy_whole_reg
memcpy_whole_reg:
    li ivl, 0x80
    vsetvli ivl, ivl, e8, LMUL, ta, ma
    csrw vstart, in_vstart
    vl8re8.v vdata, (src)
    csrw vstart, in_vstart
    vs8r.v vdata, (dst)
    ret
