/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ASM_GENERIC_SYNC_H
#define _ASM_GENERIC_SYNC_H

/* low level syncronization implementation, always as macros */

#ifndef __ASSEMBLER__

/*
 * A D(ata) M(emory) B(arier) is used when the order of memory access is
 * required, which is likely to happen in synchronization routines.
 */
#ifndef SMP_DMB
#define SMP_DMB()
#endif

/*
 * A D(ata) S(ynchronization) B(arrier) is used when previous memory access
 * need to be finished before proceeding. This is likely to happen after
 * TLB or cache operations.
 */
#ifndef SMP_DSB
#define SMP_DSB()
#endif

/*
 * An I(nstruction) S(ynchronization) B(arrier) is used to discard all
 * prefetched instructions, for example when code is loaded or when mappings
 * change.
 */
#ifndef SMP_ISB
#define SMP_ISB()
#endif

#endif /* __ASSEMBLER__ */

#endif /* _ASM_GENERIC_SYNC_H */

