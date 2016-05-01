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

#ifndef _AIM_EXPORT_H
#define _AIM_EXPORT_H

/*
 * According to linux, some toolchains use a `_' prefix for all 
 * user symbols.
 */
#ifdef HAVE_UNDERSCORE_SYMBOL_PREFIX
#define __SYMBOL(x) _##x
#define __SYMBOL_STR(x) "_" #x
#else /* ! HAVE_UNDERSCORE_SYMBOL_PREFIX */
#define __SYMBOL(x) x
#define __SYMBOL_STR(x) #x
#endif /* HAVE_UNDERSCORE_SYMBOL_PREFIX */

/* Indirect invocation to expand macros beforehand */
#define SYMBOL(x) __SYMBOL(x)
#define SYMBOL_STR(x) __SYMBOL_STR(x)

#ifndef __ASSEMBLER__

#ifdef WITH_MODULE_LOADING

struct ksym
{
	size_t value;
	const char *name;
};

#define __EXPORT_SYMBOL(sym, sec) /* TODO */

#else /* ! WITH_MODULE_LOADING */

#define __EXPORT_SYMBOL(sym, sec)

#endif /* WITH_MODULE_LOADING */

#define EXPORT_SYMBOL(sym) __EXPORT_SYMBOL(sym, "")
#define EXPORT_SYMBOL_GPL(sym) __EXPORT_SYMBOL(sym, "_gpl")
//#define EXPORT_SYMBOL_GPL_FUTURE ?
//EXPORT_UNUSED_SYMBOL ?

#endif /* __ASSEMBLER__ */

#endif /* _AIM_EXPORT_H */

