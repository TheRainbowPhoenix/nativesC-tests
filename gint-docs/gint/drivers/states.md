# states

gint:drivers:states - State structures for drivers
//
// The state structures in this header are exposed for introspection and driver
// debugging purposes. This is not part of the gint API, and there is *no
// stability guarantee* across minor and patch versions of gint.

## Data Structures

### `cpg_state_t`

Clock Pulse Generator (see cpg/cpg.c)

**Fields**:

- `uint32_t SSCGCR`

- `struct cpg_overclock_setting speed`

```c
struct cpg_state_t {
uint32_t SSCGCR;
	struct cpg_overclock_setting speed;
};
```

---

### `cpu_state_t`

CPU (see cpu/cpu.c)

**Fields**:

- `uint32_t SR`

- `uint32_t VBR`

- `uint32_t CPUOPM`

- `uint32_t rN_bank[8]`

```c
struct cpu_state_t {
uint32_t SR;
	uint32_t VBR;
	uint32_t CPUOPM;
	uint32_t rN_bank[8];
};
```

---

### `dma_state_t`

Direct Memory Access controller (see dma/dma.c)

**Fields**:

- `sh7305_dma_channel_t ch[6]`

- `uint16_t OR`

```c
struct dma_state_t {
sh7305_dma_channel_t ch[6];
	uint16_t OR;
};
```

---

### `intc_state_t`

Interrupt Controller (see intc/intc.c)

**Fields**:

- `uint16_t IPR[12]`

- `uint8_t MSK[13]`

```c
struct intc_state_t {
uint16_t IPR[12];
	uint8_t MSK[13];
};
```

---

### `mmu_state_t`

Memory Manager Unit (see mmu/mmu.c)

**Fields**:

- `uint32_t PASCR`

- `uint32_t IRMCR`

```c
struct mmu_state_t {
uint32_t PASCR;
	uint32_t IRMCR;
};
```

---

### `r61524_state_t`

R61524 display (see r61524/r61524.c)

**Fields**:

- `/* Graphics RAM range */
	uint16_t HSA, HEA, VSA, VEA`

```c
struct r61524_state_t {
/* Graphics RAM range */
	uint16_t HSA, HEA, VSA, VEA;
};
```

---

### `rtc_state_t`

Real-time Clock (see rtc/rtc.c)

**Fields**:

- `uint8_t RCR1, RCR2`

```c
struct rtc_state_t {
uint8_t RCR1, RCR2;
};
```

---

### `spu_state_t`

Sound Processing Unit (see spu/spu.c)

**Fields**:

- `uint32_t PBANKC0, PBANKC1`

- `uint32_t XBANKC0, XBANKC1`

```c
struct spu_state_t {
uint32_t PBANKC0, PBANKC1;
	uint32_t XBANKC0, XBANKC1;
};
```

---

### `t6k11_state_t`

T6K11 display (see t6k11/t6k11.c)

**Fields**:

- `/* Some status bits, obtained by using the STRD command. There are other
	   parameters that cannot be read */
	uint8_t STRD`

```c
struct t6k11_state_t {
/* Some status bits, obtained by using the STRD command. There are other
	   parameters that cannot be read */
	uint8_t STRD;
};
```

---

### `usb_state_t`

Timer Unit (see tmu/tmu.c) */
typedef struct {
	/* Individual timers; TSTR is used for ETMU */
	struct tmu_state_stored_timer {
		uint32_t TCOR;
		uint32_t TCNT;
		uint16_t TCR;
		uint16_t TSTR;
	} t[9];
	/* TSTR value for TMU */
	uint8_t TSTR;
} tmu_state_t;

/* USB 2.0 function module (see usb/usb.c)

**Fields**:

- `/* Control and power-up. We don't save power-related registers from other
	   modules nor UPONCR, because they must be changed to use the module */
	uint16_t SYSCFG, BUSWAIT, DVSTCTR, SOFCFG, TESTMODE, REG_C2`

- `/* Interrupt configuration */
	uint16_t INTENB0, INTENB1, BRDYENB, NRDYENB, BEMPENB`

- `/* Default Control Pipe */
	uint16_t DCPMAXP`

- `#ifdef GINT_USB_DEBUG
	/* Registers tracked read-only for state analysis and debugging */
	uint16_t SYSSTS, FRMNUM, UFRMNUM`

- `uint16_t CFIFOSEL, D0FIFOSEL, D1FIFOSEL, CFIFOCTR, D0FIFOCTR, D1FIFOCTR`

- `uint16_t INTSTS0, INTSTS1, BRDYSTS, NRDYSTS, BEMPSTS`

- `uint16_t DCPCFG, DCPCTR`

- `uint16_t USBADDR, USBREQ, USBVAL, USBINDX, USBLENG`

- `uint16_t PIPESEL, PIPECFG[9], PIPEnCTR[9], PIPEBUF[9]`

- `/* Ignored: UPONCR, PIPEnMAXP, PIPEnPERI, PIPEnTRN, PIPEnTRE, DEVADDn */
#endif`

```c
struct usb_state_t {
/* Control and power-up. We don't save power-related registers from other
	   modules nor UPONCR, because they must be changed to use the module */
	uint16_t SYSCFG, BUSWAIT, DVSTCTR, SOFCFG, TESTMODE, REG_C2;
	/* Interrupt configuration */
	uint16_t INTENB0, INTENB1, BRDYENB, NRDYENB, BEMPENB;
	/* Default Control Pipe */
	uint16_t DCPMAXP;

#ifdef GINT_USB_DEBUG
	/* Registers tracked read-only for state analysis and debugging */
	uint16_t SYSSTS, FRMNUM, UFRMNUM;
	uint16_t CFIFOSEL, D0FIFOSEL, D1FIFOSEL, CFIFOCTR, D0FIFOCTR, D1FIFOCTR;
	uint16_t INTSTS0, INTSTS1, BRDYSTS, NRDYSTS, BEMPSTS;
	uint16_t DCPCFG, DCPCTR;
	uint16_t USBADDR, USBREQ, USBVAL, USBINDX, USBLENG;
	uint16_t PIPESEL, PIPECFG[9], PIPEnCTR[9], PIPEBUF[9];
	/* Ignored: UPONCR, PIPEnMAXP, PIPEnPERI, PIPEnTRN, PIPEnTRE, DEVADDn */
#endif
};
```

---

## Macros

## Implementation

Source files:

- [src/dma/dma.c](https://github.com/ClasspadDev/gint/blob/dev/src/dma/dma.c)
- [src/rtc/rtc.c](https://github.com/ClasspadDev/gint/blob/dev/src/rtc/rtc.c)
- [src/r61524/r61524.c](https://github.com/ClasspadDev/gint/blob/dev/src/r61524/r61524.c)
- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/intc/intc.c](https://github.com/ClasspadDev/gint/blob/dev/src/intc/intc.c)
- [src/tmu/tmu.c](https://github.com/ClasspadDev/gint/blob/dev/src/tmu/tmu.c)
- [src/cpg/cpg.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/cpg.c)
- [src/cpu/cpu.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpu/cpu.c)
