/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 1991, 2015
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "omrport.h"
#include <unistd.h>
#include <sys/ucontext.h>
#include "omrsignal_context.h"


void
fillInUnixSignalInfo(struct OMRPortLibrary *portLibrary, void *contextInfo, struct J9UnixSignalInfo *j9Info)
{
	j9Info->platformSignalInfo.context = (ucontext_t *)contextInfo;
	/* module info is filled on demand */
}

uint32_t
infoForSignal(struct OMRPortLibrary *portLibrary, J9UnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	*name = "";

	switch (index) {

	case OMRPORT_SIG_SIGNAL_TYPE:
	case 0:
		*name = "J9Generic_Signal_Number";
		*value = &info->portLibrarySignalType;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_PLATFORM_SIGNAL_TYPE:
	case 1:
		*name = "Signal_Number";
		*value = &info->sigInfo->si_signo;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_ERROR_VALUE:
	case 2:
		*name = "Error_Value";
		*value = &info->sigInfo->si_errno;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_CODE:
	case 3:
		*name = "Signal_Code";
		*value = &info->sigInfo->si_code;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_HANDLER:
	case 4:
		*name = "Handler1";
		*value = &info->handlerAddress;
		return OMRPORT_SIG_VALUE_ADDRESS;

	case 5:
		*name = "Handler2";
		*value = &info->handlerAddress2;
		return OMRPORT_SIG_VALUE_ADDRESS;

	case OMRPORT_SIG_SIGNAL_INACCESSIBLE_ADDRESS:
	case 6:
		/* si_code > 0 indicates that the signal was generated by the kernel */
		if (info->sigInfo->si_code > 0) {
			if ((info->sigInfo->si_signo == SIGBUS) || (info->sigInfo->si_signo == SIGSEGV)) {
				*name = "InaccessibleAddress";
				*value = &info->sigInfo->si_addr;
				return OMRPORT_SIG_VALUE_ADDRESS;
			}
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;

	default:
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}

uint32_t
infoForFPR(struct OMRPortLibrary *portLibrary, J9UnixSignalInfo *info, int32_t index, const char **name, void **value)
{
#define NXMMREGS 16

	uint64_t *xmmRegStructElement64;
	struct sigcontext *context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;

	if (context->fpstate == NULL) {
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}

	const char *n_xmm[NXMMREGS] = {	"xmm0",
									"xmm1",
									"xmm2",
									"xmm3",
									"xmm4",
									"xmm5",
									"xmm6",
									"xmm7",
									"xmm8",
									"xmm9",
									"xmm10",
									"xmm11",
									"xmm12",
									"xmm13",
									"xmm14",
									"xmm15"
								  };


	if ((index >= 0) && (index < NXMMREGS)) {
		*name = n_xmm[index];
		xmmRegStructElement64 = ((uint64_t *)&(context->fpstate->_xmm[index].element[0]));
		*value = (void *) xmmRegStructElement64;
		return OMRPORT_SIG_VALUE_FLOAT_64;
	} else {
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}

uint32_t
infoForGPR(struct OMRPortLibrary *portLibrary, J9UnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	*name = "";

	switch (index) {
	case OMRPORT_SIG_GPR_AMD64_RDI:
	case 0:
		*name = "RDI";
		*value = &context->rdi;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_RSI:
	case 1:
		*name = "RSI";
		*value = &context->rsi;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_RAX:
	case 2:
		*name = "RAX";
		*value = &context->rax;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_RBX:
	case 3:
		*name = "RBX";
		*value = &context->rbx;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_RCX:
	case 4:
		*name = "RCX";
		*value = &context->rcx;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_RDX:
	case 5:
		*name = "RDX";
		*value = &context->rdx;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R8:
	case 6:
		*name = "R8";
		*value = &context->r8;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R9:
	case 7:
		*name = "R9";
		*value = &context->r9;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R10:
	case 8:
		*name = "R10";
		*value = &context->r10;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R11:
	case 9:
		*name = "R11";
		*value = &context->r11;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R12:
	case 10:
		*name = "R12";
		*value = &context->r12;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R13:
	case 11:
		*name = "R13";
		*value = &context->r13;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R14:
	case 12:
		*name = "R14";
		*value = &context->r14;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case OMRPORT_SIG_GPR_AMD64_R15:
	case 13:
		*name = "R15";
		*value = &context->r15;
		return OMRPORT_SIG_VALUE_ADDRESS;
	default:
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}

uint32_t
infoForControl(struct OMRPortLibrary *portLibrary, J9UnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	*name = "";

	switch (index) {
	case OMRPORT_SIG_CONTROL_PC:
	case 0:
		*name = "RIP";
		*value = (void *)&(context->rip);
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 1:
		*name = "GS";
		*value = (void *)&(context->gs);
		return OMRPORT_SIG_VALUE_16;
	case 2:
		*name = "FS";
		*value = (void *)&(context->fs);
		return OMRPORT_SIG_VALUE_16;
	case OMRPORT_SIG_CONTROL_SP:
	case 3:
		*name = "RSP";
		*value = (void *)&(context->rsp);
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 4:
		*name = "EFlags";
		*value = (void *)&(context->eflags);
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 5:
		*name = "CS";
		*value = (void *)&(context->cs);
		return OMRPORT_SIG_VALUE_16;
	case OMRPORT_SIG_CONTROL_BP:
	case 6:
		*name = "RBP";
		*value = &context->rbp;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 7:
		*name = "ERR";
		*value = &context->err;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 8:
		*name = "TRAPNO";
		*value = &context->trapno;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 9:
		*name = "OLDMASK";
		*value = &context->oldmask;
		return OMRPORT_SIG_VALUE_ADDRESS;
	case 10:
		*name = "CR2";
		*value = &context->cr2;
		return OMRPORT_SIG_VALUE_ADDRESS;
	default:
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}

uint32_t
infoForModule(struct OMRPortLibrary *portLibrary, J9UnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	Dl_info *dl_info = &(info->platformSignalInfo.dl_info);
	struct sigcontext *context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	*name = "";

	int dl_result = dladdr((void *)context->rip, dl_info);

	switch (index) {
	case OMRPORT_SIG_MODULE_NAME:
	case 0:
		*name = "Module";
		if (dl_result) {
			*value = (void *)(dl_info->dli_fname);
			return OMRPORT_SIG_VALUE_STRING;
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;
	case 1:
		*name = "Module_base_address";
		if (dl_result) {
			*value = (void *)&(dl_info->dli_fbase);
			return OMRPORT_SIG_VALUE_ADDRESS;
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;
	case 2:
		*name = "Symbol";
		if (dl_result) {
			if (dl_info->dli_sname != NULL) {
				*value = (void *)(dl_info->dli_sname);
				return OMRPORT_SIG_VALUE_STRING;
			}
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;
	case 3:
		*name = "Symbol_address";
		if (dl_result) {
			*value = (void *)&(dl_info->dli_saddr);
			return OMRPORT_SIG_VALUE_ADDRESS;
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;
	default:
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}