#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=int.c LCD.c main.c taskdesc.c tsk_task0.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/int.o ${OBJECTDIR}/LCD.o ${OBJECTDIR}/main.o ${OBJECTDIR}/taskdesc.o ${OBJECTDIR}/tsk_task0.o
POSSIBLE_DEPFILES=${OBJECTDIR}/int.o.d ${OBJECTDIR}/LCD.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/taskdesc.o.d ${OBJECTDIR}/tsk_task0.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/int.o ${OBJECTDIR}/LCD.o ${OBJECTDIR}/main.o ${OBJECTDIR}/taskdesc.o ${OBJECTDIR}/tsk_task0.o

# Source Files
SOURCEFILES=int.c LCD.c main.c taskdesc.c tsk_task0.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F4620
MP_PROCESSOR_OPTION_LD=18f4620
MP_LINKER_DEBUG_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/int.o: int.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/int.o.d 
	@${RM} ${OBJECTDIR}/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/int.o   int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/int.o 
	@${FIXDEPS} "${OBJECTDIR}/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/LCD.o: LCD.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD.o.d 
	@${RM} ${OBJECTDIR}/LCD.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/LCD.o   LCD.c 
	@${DEP_GEN} -d ${OBJECTDIR}/LCD.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/taskdesc.o: taskdesc.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/taskdesc.o   taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/tsk_task0.o: tsk_task0.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/tsk_task0.o.d 
	@${RM} ${OBJECTDIR}/tsk_task0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/tsk_task0.o   tsk_task0.c 
	@${DEP_GEN} -d ${OBJECTDIR}/tsk_task0.o 
	@${FIXDEPS} "${OBJECTDIR}/tsk_task0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/int.o: int.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/int.o.d 
	@${RM} ${OBJECTDIR}/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/int.o   int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/int.o 
	@${FIXDEPS} "${OBJECTDIR}/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/LCD.o: LCD.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD.o.d 
	@${RM} ${OBJECTDIR}/LCD.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/LCD.o   LCD.c 
	@${DEP_GEN} -d ${OBJECTDIR}/LCD.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/taskdesc.o: taskdesc.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/taskdesc.o   taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/tsk_task0.o: tsk_task0.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/tsk_task0.o.d 
	@${RM} ${OBJECTDIR}/tsk_task0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../Downloads/PICos18_v2/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/tsk_task0.o   tsk_task0.c 
	@${DEP_GEN} -d ${OBJECTDIR}/tsk_task0.o 
	@${FIXDEPS} "${OBJECTDIR}/tsk_task0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../../../Downloads/PICos18_v2/Kernel/picos18.lib  ../../../../../../Downloads/PICos18_v2/Linker/18f4620.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "../../../../../../Downloads/PICos18_v2/Linker/18f4620.lkr"    -x -u_DEBUG -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"../../../../../../Downloads/PICos18_v2/Kernel"     $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  ../../../../../../Downloads/PICos18_v2/Kernel/picos18.lib 
else
dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../../../Downloads/PICos18_v2/Kernel/picos18.lib ../../../../../../Downloads/PICos18_v2/Linker/18f4620.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "../../../../../../Downloads/PICos18_v2/Linker/18f4620.lkr"     -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"../../../../../../Downloads/PICos18_v2/Kernel"    -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/lcdSample.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  ../../../../../../Downloads/PICos18_v2/Kernel/picos18.lib 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default
