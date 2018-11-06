# <center> Summary of *cppcheck*

----------------------------
## <center> CONTENTS </center> ##
1. Introduction
2. Rules Summary
 - 2.1 Classification
 - 2.2 List of Rules  
 - 2.3 Main Type
 - 2.4 Detail Information
3. Relevant Documentation

## <center> TEXT </center> ##

## 1. Introduction

This is a brief summary on rules of *cppcheck*.

*Cppcheck* is an analysis tool for C/C++ code. Unlike C/C++ compilers and many other analysis tools, it doesn't detect syntax errors. Instead, *Cppcheck* detects the types of bugs that the compilers normally fail to detect. The goal is no false positives.  

Supported code and platforms:    
- You can check non-standard code that includes various compiler extensions, inline assembly code, etc.    
- *Cppcheck* should be compilable by any C++ compiler that handles the latest C++ standard.  
- *Cppcheck* should work on any platform that has sufficient CPU and memory.  

Please understand that there are limits of *Cppcheck*. *Cppcheck* is rarely wrong about reported errors. But there are many bugs that it doesn't detect.  

You will find more bugs in your software by testing your software carefully, than by using *Cppcheck*.  
You will find more bugs in your software by instrumenting your software, than by using *Cppcheck*. But *Cppcheck* can still detect some of the bugs that you miss when testing and instrumenting your software.  

## 2. Rules Summary

### 2.1 Classification

**Auto Variables**  
A pointer to a variable is only valid as long as the variable is in scope.   
[autoVariables, cwe = 562](checkingRules/error_autoVariables_CWE562.md)  
[returnAddressOfAutoVariable, cwe = 562](checkingRules/error_returnAddressOfAutoVariable_CWE562.md)  
[returnLocalVariable, cwe = 562](checkingRules/error_returnLocalVariable_CWE562.md)  
[returnReference, cwe = 562](checkingRules/error_returnReference_CWE562.md)  
[returnTempReference, cwe = 562](checkingRules/error_returnTempReference_CWE562.md)  
[autovarInvalidDeallocation, cwe = 590](checkingRules/error_autovarInvalidDeallocation_CWE590.md)  
[returnAddressOfFunctionParameter, cwe = 562](checkingRules/error_returnAddressOfFunctionParameter_CWE562.md)  

**buffer overrun**  
[negativeMemoryAllocationSize, cwe = 131](checkingRules/error_negativeMemoryAllocationSize_CWE131.md)  
[mismatchSize, cwe = 131](checkingRules/error_mismatchSize_CWE131.md)  
[negativeArraySize, cwe = 758](checkingRules/error_negativeArraySize_CWE758.md)  
[negativeIndex, cwe = 786](checkingRules/error_negativeIndex_CWE786.md)  
[arrayIndexOutOfBounds, cwe = 788](checkingRules/error_arrayIndexOutOfBounds_CWE119.md)  
[bufferAccessOutOfBounds, cwe = 788](checkingRules/error_bufferAccessOutOfBounds_CWE788.md)  
[insecureCmdLineArgs, cwe = 119](checkingRules/error_insecureCmdLineArgs_CWE119.md)  
[integerOverflow, cwe = 190](checkingRules/error_integerOverflow_CWE190.md)  
[floatConversionOverflow, cwe = 190](checkingRules/error_floatConversionOverflow_CWE190.md)  
[uninitstring, cwe = 676](checkingRules/error_uninitstring_CWE676.md)  

**memory leak & use after free & resource leak**  
[memleak, cwe = 401](checkingRules/error_memleak_CWE401.md)  
[memleakOnRealloc, cwe = 401](checkingRules/error_memleakOnRealloc_CWE401.md)  
[leakNoVarFunctionCall, cwe = 772](checkingRules/error_leakNoVarFunctionCall_CWE772.md)  
[leakReturnValNotUsed, cwe = 771](checkingRules/error_leakReturnValNotUsed_CWE771.md)  
[doubleFree, cwe = 415](checkingRules/error_doubleFree_CWE415.md)  
[deallocDealloc, cwe = 415](checkingRules/error_deallocDealloc_CWE415.md)  
[deallocret, cwe = 672](checkingRules/error_deallocret_CWE672.md)  
[deallocuse, cwe = 416](checkingRules/error_deallocuse_CWE416.md)  
[resourceLeak, cwe = 775](checkingRules/error_resourceLeak_CWE775.md)  
[useClosedFile, cwe = 910](checkingRules/error_useClosedFile_CWE910.md)  
[mismatchAllocDealloc, cwe = 762](checkingRules/error_mismatchAllocDealloc_CWE762.md)  
[unusedAllocatedMemory, cwe = 563](checkingRules/style_unusedAllocatedMemory_CWE563.md)  
[publicAllocationError, cwe = 398](checkingRules/style_publicAllocationError_CWE398.md)  

**Type Error**  
[assignBoolToPointer, cwe = 587](checkingRules/error_assignBoolToPointer_CWE587.md)  
[pointerArithBool, cwe = 571](checkingRules/error_pointerArithBool_CWE571.md)  
[invalidFunctionArg, cwe = 628](checkingRules/error_invalidFunctionArg_CWE628.md)  
[invalidFunctionArgBool, cwe = 628](checkingRules/error_invalidFunctionArgBool_CWE628.md)  
[operatorEqMissingReturnStatement, cwe = 398](checkingRules/error_operatorEqMissingReturnStatement_CWE398.md)  
[strPlusChar, cwe = 665](checkingRules/error_strPlusChar_CWE665.md)  
[uninitvar, cwe = 908](checkingRules/error_uninitvar_CWE908.md)  
[AssignmentAddressToInteger, cwe = 758](checkingRules/portability_AssignmentAddressToInteger_CWE758.md)  
[AssignmentIntegerToAddress, cwe = 758](checkingRules/portability_AssignmentIntegerToAddress_CWE758.md)  
[CastIntegerToAddressAtReturn, cwe = 758](checkingRules/portability_CastIntegerToAddressAtReturn_CWE758.md)  
[CastAddressToIntegerAtReturn, cwe = 758](checkingRules/portability_CastAddressToIntegerAtReturn_CWE758.md)  
[uselessAssignmentArg, cwe = 398](checkingRules/style_uselessAssignmentArg_CWE398.md)  
[assignBoolToFloat, cwe = 704](checkingRules/style_assignBoolToFloat_CWE704.md)  
[ignoredReturnValue, cwe = 252](checkingRules/warning_ignoredReturnValue_CWE252.md)  
[compareBoolExpressionWithInt, cwe = 398](checkingRules/warning_compareBoolExpressionWithInt_CWE398.md)  

**functions usage**
[wrongPipeParameterSize, cwe = 686](checkingRules/error_wrongPipeParameterSize_CWE686.md)  
[uselessAssignmentPtrArg, cwe = 398](checkingRules/warning_uselessAssignmentPtrArg_CWE398.md)  
[memsetFloat, cwe = 688](checkingRules/portability_memsetFloat_CWE668.md)  
[memsetClassFloat, cwe = 758](checkingRules/portability_memsetClassFloat_CWE758.md)  
[sizeofCalculation, cwe = 682](checkingRules/warning_sizeofCalculation_CWE682.md)  
[strncatUsage, cwe = 119](checkingRules/error_strncatUsage_CWE119.md)  
[asctimeCalled, cwe = 477](checkingRules/style_asctimeCalled_CWE477.md)  
[getsCalled, cwe = 477](checkingRules/style_getsCalled_CWE477.md)  

**Null pointer**  
[nullPointer, cwe = 476](checkingRules/warning_nullPointer_CWE476.md)  
[nullPointerRedundantCheck, cwe = 476](checkingRules/warning_nullPointerRedundantCheck_CWE476.md)  

**Check class**  
[virtualDestructor, cwe = 404](checkingRules/error_virtualDestructor_CWE404.md)  
[memsetClassReference, cwe = 665](checkingRules/error_memsetClassReference_CWE665.md)   
[mallocOnClassError, cwe = 665](checkingRules/error_mallocOnClassError_CWE665.md)  
[selfInitialization, cwe = 665](checkingRules/error_selfInitialization_CWE665.md)  
[memsetClass, cwe = 762](checkingRules/error_mismatchAllocDealloc_CWE762.md)  
[noConstructor, cwe = 398](checkingRules/style_noConstructor_CWE398.md)  
[noExplicitConstructor, cwe = 398](checkingRules/style_noExplicitConstructor_CWE398.md)  
[noCopyConstructor, cwe = 398](checkingRules/style_noCopyConstructor_CWE398.md)  
[memsetClassFloat, cwe = 758](checkingRules/portability_memsetClassFloat_CWE758.md)  
[uninitStructMember, cwe = 908](checkingRules/error_uninitStructMember_CWE908.md)  
[uninitdata, cwe = 908](checkingRules/error_uninitdata_CWE908.md)  
[unusedPrivateFunction, cwe = 398](checkingRules/style_unusedPrivateFunction_CWE398.md)
[unusedStructMember, cwe = 563](checkingRules/style_unusedStructMember_CWE563.md)  
[unusedScopedObject, cwe = 563](checkingRules/style_unusedScopedObject_CWE563.md)  
[uninitMemberVar, cwe = 398](checkingRules/warning_uninitMemberVar_CWE398.md)  
[duplInheritedMember, cwe = 398](checkingRules/warning_duplInheritedMember_CWE398.md)  
[exceptThrowInDestructor, cwe = 398](checkingRules/warning_exceptThrowInDestructor_CWE398.md)  

**IO Error**  
[IOWithoutPositioning, cwe = 664](checkingRules/error_IOWithoutPositioning_CWE664.md)  
[coutCerrMisusage, cwe = 398](checkingRules/error_coutCerrMisusage_CWE398.md)  
[readWriteOnlyFile, cwe = 664](checkingRules/error_readWriteOnlyFile_CWE664.md)  
[writeReadOnlyFile, cwe = 664](checkingRules/error_writeReadOnlyFile_CWE664.md)  
[useClosedFile, cwe = 910](checkingRules/error_useClosedFile_CWE910.md)  
[wrongPrintfScanfArgNum, cwe = 685](checkingRules/error_wrongPrintfScanfArgNum_CWE685.md)  
[invalidScanfFormatWidth, cwe = 687](checkingRules/error_invalidScanfFormatWidth_CWE687.md)  
[sprintfOverlappingData, cwe = 628](checkingRules/error_sprintfOverlappingData_CWE628.md)  
[invalidscanf, cwe = 119](checkingRules/warning_invalidScanf_CWE687.md)  
[invalidPrintfArgType_sint, cwe = 686](checkingRules/portability_invalidPrintfArgType_sint_CWE686.md)  

**Auto Pointer**  
[useAutoPointerContainer, cwe = 664](checkingRules/error_useAutoPointerContainer_CWE664.md)  
[useAutoPointerArray, cwe = 664](checkingRules/error_useAutoPointerArray_CWE664.md)  
[useAutoPointerMalloc, cwe = 762](checkingRules/error_useAutoPointerMalloc_CWE762.md)  
[useAutoPointerCopy, cwe = 398](checkingRules/style_useAutoPointerCopy_CWE398.md)  

**STL usage**  
Check for invalid usage of STL:  
[boostForeachError, cwe = 664](checkingRules/error_boostForeachError_CWE664.md)  
[stlOutOfBounds, cwe = 788](checkingRules/error_stlOutOfBounds_CWE788.md)  
[stlBoundaries, cwe = 664](checkingRules/error_stlBoundaries_CWE664.md)  
[stlcstr, cwe = 664](checkingRules/error_stlcstr_CWE664.md)  
[stlcstrReturn, cwe = 704](checkingRules/performance_stlcstrReturn_CWE704.md)  
[stlcstrParam, cwe = 704](checkingRules/performance_stlcstrParam_CWE704.md)  
[negativeIndex, cwe = 786](checkingRules/error_negativeIndex_CWE786.md)  
[invalidIterator1, cwe = 664](checkingRules/error_invalidIterator1_CWE664.md)  
[invalidIterator2, cwe = 664](checkingRules/error_invalidIterator2_CWE664.md)  
[eraseDereference, cwe = 664](checkingRules/error_eraseDereference_CWE664.md)  

**va_list**  
[va_start_referencePassed, cwe = 758](checkingRules/error_va_start_referencePassed_CWE758.md)  
[va_end_missing, cwe = 664](checkingRules/error_va_end_missing_CWE664.md)  
[va_list_usedBeforeStarted, cwe = 664](checkingRules/error_va_list_usedBeforeStarted_CWE664.md)  
[va_start_subsequentCalls, cwe = 664](checkingRules/error_va_start_subsequentCalls_CWE664.md)  

**shift**  
[shiftNegative, cwe = 758](checkingRules/error_shiftNegative_CWE758.md)  
[shiftTooManyBits, cwe = 758](checkingRules/error_shiftTooManyBits_CWE758.md)  
[shiftTooManyBitsSigned, cwe = 758](checkingRules/error_shiftTooManyBitsSigned_CWE758.md)  
[shiftNegativeLHS, cwe = 758](checkingRules/portability_shiftNegativeLHS_CWE758.md)  

**Other check**
[zerodiv, cwe = 369](checkingRules/error_zerodiv_CWE369.md)  
[zerodivcond, cwe = 369](checkingRules/error_zerodivcond_CWE369.md)
[raceAfterInterlockedDecrement, cwe = 362](checkingRules/error_raceAfterInterlockedDecrement_CWE362.md)  
[unknownEvaluationOrder, cwe = 768](checkingRules/error_unknownEvaluationOrder_CWE768.md)  
[postfixOperator, cwe = 398](checkingRules/performance_postfixOperator_CWE398.md)  
[knownConditionTrueFalse, cwe = 570](checkingRules/style_knownConditionTrueFalse_CWE570.md)  
[unreachableCode, cwe = 561](checkingRules/style_unreachableCode_CWE561.md)  
[cstyleCast, cwe = 398](checkingRules/style_cstyleCast_CWE398.md)  
[redundantAssignment, cwe = 563](checkingRules/style_redundantAssignment_CWE563.md)  
[constStatement, cwe = 398](checkingRules/warning_constStatement_CWE398.md)  
[unusedFunction, cwe = 561](checkingRules/style_unusedFunction_CWE561.md)  
[unusedVariable, cwe = 563](checkingRules/style_unusedVariable_CWE563.md)  
[unusedAllocatedMemory, cwe = 563](checkingRules/style_unusedAllocatedMemory_CWE563.md)  
[unreadVariable, cwe = 563](checkingRules/style_unreadVariable_CWE563.md)  
[unassignedVariable, cwe = 665](checkingRules/style_unassignedVariable_CWE665.md)  
[unusedStructMember, cwe = 563](checkingRules/style_unusedStructMember_CWE563.md)  
[duplicateBreak, cwe = 561](checkingRules/style_duplicateBreak_CWE561.md)  
[selfAssignment, cwe = 398](../checkingRules/warning_selfAssignment_CWE398.md)  
[duplicateExpression, cwe = 398](checkingRules/style_duplicateExpression_CWE398.md)  

### 2.2 List of Rules

**error**  
1. [autoVariables, cwe = 562](checkingRules/error_autoVariables_CWE562.md)  
2. [returnAddressOfAutoVariable, cwe = 562](checkingRules/error_returnAddressOfAutoVariable_CWE562.md)  
3. [returnLocalVariable, cwe = 562](checkingRules/error_returnLocalVariable_CWE562.md)  
4. [returnReference, cwe = 562](checkingRules/error_returnReference_CWE562.md)  
5. [returnTempReference, cwe = 562](checkingRules/error_returnTempReference_CWE562.md)  
6. [autovarInvalidDeallocation, cwe = 590](checkingRules/error_autovarInvalidDeallocation_CWE590.md)  
7. [returnAddressOfFunctionParameter, cwe = 562](checkingRules/error_returnAddressOfFunctionParameter_CWE562.md)  
8. [assignBoolToPointer, cwe = 587](checkingRules/error_assignBoolToPointer_CWE587.md)  
9. [pointerArithBool, cwe = 571](checkingRules/error_pointerArithBool_CWE571.md)  
10. [boostForeachError, cwe = 664](checkingRules/error_boostForeachError_CWE664.md)  
11. [arrayIndexOutOfBounds, cwe = 788](checkingRules/error_arrayIndexOutOfBounds_CWE119.md)  
12. [bufferAccessOutOfBounds, cwe = 788](checkingRules/error_bufferAccessOutOfBounds_CWE788.md)  
13. outOfBounds, cwe = 788, ????????????????????????????  
14. [negativeIndex, cwe = 786](checkingRules/error_negativeIndex_CWE786.md)  
15. [insecureCmdLineArgs, cwe = 119](checkingRules/error_insecureCmdLineArgs_CWE119.md)  
16. [negativeMemoryAllocationSize, cwe = 131](checkingRules/error_negativeMemoryAllocationSize_CWE131.md)  
17. [negativeArraySize, cwe = 758](checkingRules/error_negativeArraySize_CWE758.md)  
18. [invalidFunctionArg, cwe = 628](checkingRules/error_invalidFunctionArg_CWE628.md)  
19. [invalidFunctionArgBool, cwe = 628](checkingRules/error_invalidFunctionArgBool_CWE628.md)  
20. [memsetClass, cwe = 762](checkingRules/error_mismatchAllocDealloc_CWE762.md)  
21. [memsetClassReference, cwe = 665](checkingRules/error_memsetClassReference_CWE665.md)  
22. [mallocOnClassError, cwe = 665](checkingRules/error_mallocOnClassError_CWE665.md)  
23. [virtualDestructor, cwe = 404](checkingRules/error_virtualDestructor_CWE404.md)  
24. [operatorEqMissingReturnStatement, cwe = 398](checkingRules/error_operatorEqMissingReturnStatement_CWE398.md)  
25. [selfInitialization, cwe = 665](checkingRules/error_selfInitialization_CWE665.md)  
26. throwInNoexceptFunction, cwe = 398, ????????????????????????????  
27. [coutCerrMisusage, cwe = 398](checkingRules/error_coutCerrMisusage_CWE398.md)  
28. [IOWithoutPositioning, cwe = 664](checkingRules/error_IOWithoutPositioning_CWE664.md)  
29. [readWriteOnlyFile, cwe = 664](checkingRules/error_readWriteOnlyFile_CWE664.md)  
30. [writeReadOnlyFile, cwe = 664](checkingRules/error_writeReadOnlyFile_CWE664.md)  
31. [useClosedFile, cwe = 910](checkingRules/error_useClosedFile_CWE910.md)  
32. [wrongPrintfScanfArgNum, cwe = 685](checkingRules/error_wrongPrintfScanfArgNum_CWE685.md)  
33. [invalidScanfFormatWidth, cwe = 687](checkingRules/error_invalidScanfFormatWidth_CWE687.md)  
34. [deallocret, cwe = 672](checkingRules/error_deallocret_CWE672.md)  
35. [doubleFree, cwe = 415](checkingRules/error_doubleFree_CWE415.md)  
36. [leakNoVarFunctionCall, cwe = 772](checkingRules/error_leakNoVarFunctionCall_CWE772.md)  
37. [leakReturnValNotUsed, cwe = 771](checkingRules/error_leakReturnValNotUsed_CWE771.md)  
38. [memleak, cwe = 401](checkingRules/error_memleak_CWE401.md)  
39. [resourceLeak, cwe = 775](checkingRules/error_resourceLeak_CWE775.md)  
40. [deallocDealloc, cwe = 415](checkingRules/error_deallocDealloc_CWE415.md)  
41. [deallocuse, cwe = 416](checkingRules/error_deallocuse_CWE416.md)  
42. [mismatchSize, cwe = 131](checkingRules/error_mismatchSize_CWE131.md)  
43. [mismatchAllocDealloc, cwe = 762](checkingRules/error_mismatchAllocDealloc_CWE762.md)  
44. [memleakOnRealloc, cwe = 401](checkingRules/error_memleakOnRealloc_CWE401.md)  
45. [nullPointer, cwe = 476](checkingRules/warning_nullPointer_CWE476.md)  
46. nullPointerArithmetic, cwe = 682, ????????????????????????????  
47. [zerodiv, cwe = 369](checkingRules/error_zerodiv_CWE369.md)  
48. [zerodivcond, cwe = 369](checkingRules/error_zerodivcond_CWE369.md)  
49. [shiftNegative, cwe = 758](checkingRules/error_shiftNegative_CWE758.md)  
50. [wrongPipeParameterSize, cwe = 686](checkingRules/error_wrongPipeParameterSize_CWE686.md)  
51. [raceAfterInterlockedDecrement, cwe = 362](checkingRules/error_raceAfterInterlockedDecrement_CWE362.md)  
52. [unknownEvaluationOrder, cwe = 768](checkingRules/error_unknownEvaluationOrder_CWE768.md)  
53. [invalidIterator1, cwe = 664](checkingRules/error_invalidIterator1_CWE664.md)  
54. iterators, cwe = 664 , ????????????????????????????  
55. mismatchingContainers, cwe = 664 , ????????????????????????????  
56. [eraseDereference, cwe = 664](checkingRules/error_eraseDereference_CWE664.md)  
57. [stlOutOfBounds, cwe = 788](checkingRules/error_stlOutOfBounds_CWE788.md)  
58. [invalidIterator2, cwe = 664](checkingRules/error_invalidIterator2_CWE664.md)  
59. invalidPointer, cwe = 664 , ????????????????????????????  
60. [stlBoundaries, cwe = 664](checkingRules/error_stlBoundaries_CWE664.md)  
61. [stlcstr, cwe = 664](checkingRules/error_stlcstr_CWE664.md)  
62. [useAutoPointerContainer, cwe = 664](checkingRules/error_useAutoPointerContainer_CWE664.md)  
63. [useAutoPointerArray, cwe = 664](checkingRules/error_useAutoPointerArray_CWE664.md)  
64. [useAutoPointerMalloc, cwe = 762](checkingRules/error_useAutoPointerMalloc_CWE762.md)  
65. [stringLiteralWrite, cwe = 758](checkingRules/error_stringLiteralWrite_CWE758.md)  
66. [sprintfOverlappingData, cwe = 628](checkingRules/error_sprintfOverlappingData_CWE628.md)  
67. [strPlusChar, cwe = 665](checkingRules/error_strPlusChar_CWE665.md)  
68. [shiftTooManyBits, cwe = 758](checkingRules/error_shiftTooManyBits_CWE758.md)  
69. [shiftTooManyBitsSigned, cwe = 758](checkingRules/error_shiftTooManyBitsSigned_CWE758.md)  
70. [integerOverflow, cwe = 190](checkingRules/error_integerOverflow_CWE190.md)  
71. [floatConversionOverflow, cwe = 190](checkingRules/error_floatConversionOverflow_CWE190.md)  
72. [uninitstring, cwe = 676](checkingRules/error_uninitstring_CWE676.md)  
73. [uninitdata, cwe = 908](checkingRules/error_uninitdata_CWE908.md)  
74. [uninitvar, cwe = 908](checkingRules/error_uninitvar_CWE908.md)  
75. [uninitStructMember, cwe = 908](checkingRules/error_uninitStructMember_CWE908.md)  
76. deadpointer, cwe = 825 , ????????????????????????????  
77. [va_start_referencePassed, cwe = 758](checkingRules/error_va_start_referencePassed_CWE758.md)  
78. [va_end_missing, cwe = 664](checkingRules/error_va_end_missing_CWE664.md)  
79. [va_list_usedBeforeStarted, cwe = 664](checkingRules/error_va_list_usedBeforeStarted_CWE664.md)  
80. [va_start_subsequentCalls, cwe = 664](checkingRules/error_va_start_subsequentCalls_CWE664.md)  
81. [preprocessorErrorDirective]()  

**information**  
82. purgedConfiguration  
83. toomanyconfigs, cwe = 398    
84. [missingInclude](checkingRules/information_missingInclude.md)  
85. [missingIncludeSystem](checkingRules/information_missingIncludeSystem.md)  
86. ConfigurationNotChecked  

**performance**  
87. functionStatic, cwe = 398  
88. useInitializationList, cwe = 398  
89. redundantCopyLocalConst, cwe = 398  
90. redundantCopy, cwe = 563  
91. passedByValue, cwe = 398  
92. stlIfStrFind, cwe = 597  
93. [stlcstrReturn, cwe = 704](checkingRules/performance_stlcstrReturn_CWE704.md)  
94. [stlcstrParam, cwe = 704](checkingRules/performance_stlcstrParam_CWE704.md)  
95. stlSize, cwe = 398  
96. uselessCallsSwap, cwe = 628  
97. uselessCallsSubstr, cwe = 398  
98. [postfixOperator, cwe = 398](checkingRules/performance_postfixOperator_CWE398.md)  

**portability**  
99. [AssignmentAddressToInteger, cwe = 758](checkingRules/portability_AssignmentAddressToInteger_CWE758.md)  
100. [AssignmentIntegerToAddress, cwe = 758](checkingRules/portability_AssignmentIntegerToAddress_CWE758.md)  
101. [CastIntegerToAddressAtReturn, cwe = 758](checkingRules/portability_CastIntegerToAddressAtReturn_CWE758.md)  
102. [CastAddressToIntegerAtReturn, cwe = 758](checkingRules/portability_CastAddressToIntegerAtReturn_CWE758.md)  
103. pointerOutOfBounds, cwe = 398  
104. [memsetFloat, cwe = 688](checkingRules/portability_memsetFloat_CWE668.md)  
105. [memsetClassFloat, cwe = 758](checkingRules/portability_memsetClassFloat_CWE758.md)  
106. fflushOnInputStream, cwe = 398  
107. invalidPointerCast, cwe = 704  
108. [shiftNegativeLHS, cwe = 758](checkingRules/portability_shiftNegativeLHS_CWE758.md)  
109. unknownSignCharArrayIndex, cwe = 758  
110. varFuncNullUB, cwe = 475  
111. sizeofVoid, cwe = 682  
112. sizeofDereferencedVoidPointer, cwe = 682  
113. arithOperationsOnVoidPointer, cwe = 467  

**style**  
114. [uselessAssignmentArg, cwe = 398](checkingRules/style_uselessAssignmentArg_CWE398.md)  
115. [assignBoolToFloat, cwe = 704](checkingRules/style_assignBoolToFloat_CWE704.md)  
116. comparisonOfFuncReturningBoolError, cwe = 398  
117. comparisonOfTwoFuncsReturningBoolError, cwe = 398  
118. comparisonOfBoolWithBoolError, cwe = 398  
119. incrementboolean, cwe = 398  
120. bitwiseOnBoolean, cwe = 398  
121. arrayIndexThenCheck, cwe = 398  
122. unpreciseMathCall, cwe = 758  
123. [noConstructor, cwe = 398](checkingRules/style_noConstructor_CWE398.md)  
124. [noExplicitConstructor, cwe = 398](checkingRules/style_noExplicitConstructor_CWE398.md)  
125. copyCtorPointerCopying, cwe = 398  
126. [noCopyConstructor, cwe = 398](checkingRules/style_noCopyConstructor_CWE398.md)  
127. [unusedPrivateFunction, cwe = 398](checkingRules/style_unusedPrivateFunction_CWE398.md)  
128. operatorEq, cwe = 398  
129. operatorEqRetRefThis, cwe = 398  
130. operatorEqShouldBeLeftUnimplemented, cwe = 398  
131. functionConst, cwe = 398  warning_compareBoo
132. initializerList, cwe = 398  
133. unsafeClassDivZero
134. assignIfError, cwe = 398  
135. comparisonError, cwe = 398  
136. multiCondition, cwe = 398  
137. mismatchingBitAnd, cwe = 398  
138. redundantCondition, cwe = 398  
139. clarifyCondition, cwe = 398  
140. [knownConditionTrueFalse, cwe = 570](checkingRules/style_knownConditionTrueFalse_CWE570.md)  
141. exceptRethrowCopy, cwe = 398  
142. catchExceptionByValue, cwe = 398  
143. unhandledExceptionSpecification, cwe = 703  
144. unsafeClassCanLeak, cwe = 398  
145. [unusedScopedObject, cwe = 563](checkingRules/style_unusedScopedObject_CWE563.md)  
146. [redundantAssignment, cwe = 563](checkingRules/style_redundantAssignment_CWE563.md)  
147. [cstyleCast, cwe = 398](checkingRules/style_cstyleCast_CWE398.md)  
148. variableScope, cwe = 398  
149. clarifyCalculation, cwe = 783  
150. duplicateBranch, cwe = 398  
151. [duplicateExpression, cwe = 398](checkingRules/style_duplicateExpression_CWE398.md)  
152. duplicateExpressionTernary, cwe = 398  
153. [duplicateBreak, cwe = 561](checkingRules/style_duplicateBreak_CWE561.md)  
154. [unreachableCode, cwe = 561](checkingRules/style_unreachableCode_CWE561.md)  
155. unsignedLessThanZero, cwe = 570  
156. unsignedPositive, cwe = 570  
157. pointerLessThanZero, cwe = 570  
158. pointerPositive, cwe = 570  
159. nanInArithmeticExpression, cwe = 369  
160. commaSeparatedReturn, cwe = 398  
161. redundantPointerOp, cwe = 398  
162. unusedLabel, cwe = 398  
163. funcArgNamesDifferent, cwe = 628  
164. redundantIfRemove, cwe = 398  
165. [useAutoPointerCopy, cwe = 398](checkingRules/style_useAutoPointerCopy_CWE398.md)  
166. reademptycontainer, cwe = 398  
167. truncLongCastAssignment, cwe = 197  
168. truncLongCastReturn, cwe = 197  
169. [unusedFunction, cwe = 561](checkingRules/style_unusedFunction_CWE561.md)  
170. [unusedVariable, cwe = 563](checkingRules/style_unusedVariable_CWE563.md)  
171. [unusedAllocatedMemory, cwe = 563](checkingRules/style_unusedAllocatedMemory_CWE563.md)  
172. [unreadVariable, cwe = 563](checkingRules/style_unreadVariable_CWE563.md)  
173. [unassignedVariable, cwe = 665](checkingRules/style_unassignedVariable_CWE665.md)  
174. [unusedStructMember, cwe = 563](checkingRules/style_unusedStructMember_CWE563.md)  
175. [asctimeCalled, cwe = 477](checkingRules/style_asctimeCalled_CWE477.md)  
176. [getsCalled, cwe = 477](checkingRules/style_getsCalled_CWE477.md)  

**warning**  
177. assertWithSideEffect, cwe = 398  
178. assignmentInAssert, cwe = 398  
179. autoVariablesAssignGlobalPointer, cwe = 562  
180. [uselessAssignmentPtrArg, cwe = 398](checkingRules/warning_uselessAssignmentPtrArg_CWE398.md)  
181. [compareBoolExpressionWithInt, cwe = 398](checkingRules/warning_compareBoolExpressionWithInt_CWE398.md)  
182. [strncatUsage, cwe = 119](checkingRules/error_strncatUsage_CWE119.md)  
183. sizeArgumentAsChar, cwe = 682  
184. terminateStrncpy, cwe = 170  
185. bufferNotZeroTerminated, cwe = 170  
186. possibleBufferAccessOutOfBounds, cwe = 398  
187. argumentSize, cwe = 398  
188. arrayIndexOutOfBoundsCond, cwe = 119  
189. [ignoredReturnValue, cwe = 252](checkingRules/warning_ignoredReturnValue_CWE252.md)  
190. wrongmathcall, cwe = 758  
191. memsetZeroBytes, cwe = 687  
192. memsetValueOutOfRange, cwe = 686  
193. [uninitMemberVar, cwe = 398](checkingRules/warning_uninitMemberVar_CWE398.md)  
194. operatorEqVarError, cwe = 398  
195. mallocOnClassWarning, cwe = 762  
196. thisSubtraction, cwe = 398  
197. operatorEqToSelf, cwe = 398  
198. [duplInheritedMember, cwe = 398](checkingRules/warning_duplInheritedMember_CWE398.md)  
199. copyCtorAndEqOperator
200. badBitmaskCheck, cwe = 571  
201. oppositeInnerCondition, cwe = 398  
202. identicalConditionAfterEarlyExit, cwe = 398  
203. incorrectLogicOperator, cwe = 571  
204. moduloAlwaysTrueFalse, cwe = 398  
205. invalidTestForOverflow, cwe = 570  
206. pointerAdditionResultNotNull
207. [exceptThrowInDestructor, cwe = 398](checkingRules/warning_exceptThrowInDestructor_CWE398.md)  
208. exceptDeallocThrow, cwe = 398  
209. seekOnAppendedFile, cwe = 398  
210. [invalidscanf, cwe = 119](checkingRules/warning_invalidScanf_CWE687.md)  
211. invalidScanfArgType_s, cwe = 686  
212. invalidScanfArgType_int, cwe = 686  
213. invalidScanfArgType_float, cwe = 686  
214. invalidPrintfArgType_s, cwe = 686  
215. invalidPrintfArgType_n, cwe = 686  
216. invalidPrintfArgType_p, cwe = 686  
217. invalidPrintfArgType_uint, cwe = 686  
218. [invalidPrintfArgType_sint, cwe = 686](checkingRules/portability_invalidPrintfArgType_sint_CWE686.md)  
219. invalidPrintfArgType_float, cwe = 686  
220. invalidLengthModifierError, cwe = 704  
221. wrongPrintfScanfParameterPositionError, cwe = 685  
222. leakUnsafeArgAlloc, cwe = 401  
223. [publicAllocationError, cwe = 398](checkingRules/style_publicAllocationError_CWE398.md)  
224. nullPointerDefaultArg, cwe = 476  
225. [nullPointerRedundantCheck, cwe = 476](checkingRules/warning_nullPointerRedundantCheck_CWE476.md)  
226. comparisonFunctionIsAlwaysTrueOrFalse, cwe = 570  
227. checkCastIntToCharAndBack, cwe = 197  
228. [constStatement, cwe = 398](checkingRules/warning_constStatement_CWE398.md)  
229. signedCharArrayIndex, cwe = 128  
230. charBitOp, cwe = 398  
231. redundantAssignInSwitch, cwe = 563  
232. redundantCopyInSwitch, cwe = 563  
233. suspiciousCase, cwe = 398  
234. suspiciousEqualityComparison, cwe = 482  
235. [selfAssignment, cwe = 398](checkingRules/warning_selfAssignment_CWE398.md)  
236. clarifyStatement, cwe = 783  
237. suspiciousSemicolon, cwe = 398  
238. incompleteArrayFill, cwe = 131  
239. unusedLabelSwitch, cwe = 398  
240. accessMoved, cwe = 672  
241. accessForwarded, cwe = 672  
242. funcArgOrderDifferent, cwe = 683  
243. stlIfFind, cwe = 398  
244. StlMissingComparison, cwe = 834  
245. uselessCallsCompare, cwe = 628  
246. uselessCallsEmpty, cwe = 398  
247. uselessCallsRemove, cwe = 762  
248. derefInvalidIterator, cwe = 825  
249. sizeofwithsilentarraypointer, cwe = 467  
250. pointerSize, cwe = 467  
251. sizeofDivisionMemfunc, cwe = 682  
252. sizeofwithnumericparameter, cwe = 682  
253. sizeofsizeof, cwe = 682  
254. [sizeofCalculation, cwe = 682](checkingRules/warning_sizeofCalculation_CWE682.md)  
255. sizeofFunctionCall, cwe = 682  
256. multiplySizeof, cwe = 682  
257. divideSizeof, cwe = 682  
258. incorrectStringCompare, cwe = 570  
259. literalWithCharPtrCompare, cwe = 595  
260. charLiteralWithCharPtrCompare, cwe = 595  
261. incorrectStringBooleanError, cwe = 571  
262. staticStringCompare, cwe = 570  
263. stringCompare, cwe = 571  
264. overlappingStrcmp
265. signConversion, cwe = 195  
266. va_start_wrongParameter, cwe = 688  


### 2.3 Main Type

*Cppcheck* has many different kinds of checks. A full list is available.

**64-bit portability**  
Check if there is 64-bit portability issues:  
- assign address to/from int/long  
- casting address from/to integer when returning from function  

**Assert**  
Warn if there are side effects in assert statements (since this cause different behaviour in debug/release builds).

**Auto Variables**  
A pointer to a variable is only valid as long as the variable is in scope.  
Check:  
- returning a pointer to auto or temporary variable  
- assigning address of an variable to an effective parameter of a function    
- returning reference to local/temporary variable  
- returning address of function parameter  
- suspicious assignment of pointer argument  
- useless assignment of function argument  

**Boolean**  
Boolean type checks  
- using increment on boolean  
- comparison of a boolean expression with an integer other than 0 or 1  
- comparison of a function returning boolean value using relational operator  
- comparison of a boolean value with boolean value using relational operator  
- using bool in bitwise expression  
- pointer addition in condition (either dereference is forgot or pointer overflow is required to make the condition false)  
- Assigning bool value to pointer or float  

**Boost usage**  
Check for invalid usage of Boost:  
- container modification during BOOST_FOREACH  

**Bounds checking**  
Out of bounds checking:  
- Array index out of bounds detection by value flow analysis  
- Dangerous usage of strncat()  
- char constant passed as size to function like memset()  
- strncpy() leaving string unterminated  
- Accessing array with negative index  
- Unsafe usage of main(argv, argc) arguments  
- Accessing array with index variable before checking its value  
- Check for large enough arrays being passed to functions  
- Allocating memory with a negative size  

**Check function usage**  
Check function usage:  
- return value of certain functions not used  
- invalid input values for functions  
- Warn if a function is called whose usage is discouraged  
- memset() third argument is zero  
- memset() with a value out of range as the 2nd parameter  
- memset() with a float as the 2nd parameter  

**Class**  
Check the code for each class.  
- Missing constructors and copy constructors  
- Constructors which should be explicit  
- Are all variables initialized by the constructors?  
- Are all variables assigned by 'operator='?  
- Warn if memset, memcpy etc are used on a class  
- Warn if memory for classes is allocated with malloc()  
- If it's a base class, check that the destructor is virtual  
- Are there unused private functions?  
- 'operator=' should return reference to self  
- 'operator=' should check for assignment to self  
- Constness for member functions  
- Order of initializations  
- Suggest usage of initialization list  
- Initialization of a member with itself  
- Suspicious subtraction from 'this'  
- Call of pure virtual function in constructor/destructor  
- Duplicated inherited data members  
- If 'copy constructor' defined, 'operator=' also should be defined and vice versa  
- Check that arbitrary usage of public interface does not result in division by zero  

**Condition**  
Match conditions with assignments and other conditions:  
- Mismatching assignment and comparison => comparison is always true/false  
- Mismatching lhs and rhs in comparison => comparison is always true/false  
- Detect usage of | where & should be used  
- Detect matching 'if' and 'else if' conditions  
- Mismatching bitand (a &= 0xf0; a &= 1; => a = 0)  
- Opposite inner condition is always false  
- Identical condition after early exit is always false  
- Condition that is always true/false  
- Mutual exclusion over || always evaluating to true  
- Comparisons of modulo results that are always true/false.  
- Known variable values => condition is always true/false  
- Invalid test for overflow (for example 'ptr+u < ptr'). Condition is always false unless there is overflow, and overflow is undefined behaviour.  

**Exception Safety**  
Checking exception safety  
- Throwing exceptions in destructors  
- Throwing exception during invalid state  
- Throwing a copy of a caught exception instead of rethrowing the original exception  
- Exception caught by value instead of by reference  
- Throwing exception in noexcept, nothrow(), __attribute__((nothrow)) or __declspec(nothrow) function  
- Unhandled exception specification when calling function foo()  

**IO using format string**  
Check format string input/output operations.  
- Bad usage of the function 'sprintf' (overlapping data)  
- Missing or wrong width specifiers in 'scanf' format string  
- Use a file that has been closed  
- File input/output without positioning results in undefined behaviour  
- Read to a file that has only been opened for writing (or vice versa)  
- Repositioning operation on a file opened in append mode  
- Using fflush() on an input stream  
- Invalid usage of output stream. For example: 'std::cout << std::cout;'  
- Wrong number of arguments given to 'printf' or 'scanf;'  

**Leaks (auto variables)**  
Detect when a auto variable is allocated but not deallocated or deallocated twice.  

**Memory leaks (address not taken)**  
Not taking the address to allocated memory  

**Memory leaks (class variables)**  
If the constructor allocate memory then the destructor must deallocate it.  

**Memory leaks (function variables)**  
Is there any allocated memory when a function goes out of scope  

**Memory leaks (struct members)**  
Don't forget to deallocate struct members  

**Null pointer**  
Null pointers  
- null pointer dereferencing  
- undefined null pointer arithmetic  

**Other**  
Other checks  
- division with zero  
- scoped object destroyed immediately after construction  
- assignment in an assert statement  
- free() or delete of an invalid memory location  
- bitwise operation with negative right operand  
- provide wrong dimensioned array to pipe() system command (--std=posix)  
- cast the return values of getc(),fgetc() and getchar() to character and compare it to EOF  
- race condition with non-interlocked access after InterlockedDecrement() call  
- expression 'x = x++;' depends on order of evaluation of side effects  
- either division by zero or useless condition  
- access of moved or forwarded variable.  
- redundant data copying for const variable  
- subsequent assignment or copying to a variable or buffer  
- passing parameter by value  
- Passing NULL pointer to function with variable number of arguments leads to UB.  
- C-style pointer cast in C++ code  
- casting between incompatible pointer types  
- Incomplete statement(IncompleteStatement)  
- check how signed char variables are used(CharVar)  
- variable scope can be limited  
- unusual pointer arithmetic. For example: "abc" + 'd'  
- redundant assignment, increment, or bitwise operation in a switch statement  
- redundant strcpy in a switch statement  
- Suspicious case labels in switch()  
- assignment of a variable to itself  
- Comparison of values leading always to true or false  
- Clarify calculation with parentheses  
- suspicious comparison of '\0' with a char* variable  
- duplicate break statement  
- unreachable code  
- testing if unsigned variable is negative/positive  
- Suspicious use of ; at the end of 'if/for/while' statement.  
- Array filled incompletely using memset/memcpy/memmove.  
- NaN (not a number) value used in arithmetic expression.  
- comma in return statement (the comma can easily be misread as a semicolon).  
- prefer erfc, expm1 or log1p to avoid loss of precision.  
- identical code in both branches of if/else or ternary operator.  
- redundant pointer operation on pointer like &*some_ptr.  
- find unused 'goto' labels.  
- function declaration and definition argument names different.  
- function declaration and definition argument order different.  

**STL usage**  
Check for invalid usage of STL:  
- out of bounds errors  
- misuse of iterators when iterating through a container  
- mismatching containers in calls  
- dereferencing an erased iterator  
- for vectors: using iterator/pointer after push_back has been used  
- optimisation: use empty() instead of size() to guarantee fast code  
- suspicious condition when using find  
- redundant condition  
- common mistakes when using string::c_str()  
- using auto pointer (auto_ptr)  
- useless calls of string and STL functions  
- dereferencing an invalid iterator  
- reading from empty STL container  

**Sizeof**  
sizeof() usage checks  
- sizeof for array given as function argument  
- sizeof for numeric given as function argument  
- using sizeof(pointer) instead of the size of pointed data  
- look for 'sizeof sizeof ..'  
- look for calculations inside sizeof()  
- look for function calls inside sizeof()  
- look for suspicious calculations with sizeof()  
- using 'sizeof(void)' which is undefined  

**String**  
Detect misusage of C-style strings:  
- overlapping buffers passed to sprintf as source and destination  
- incorrect length arguments for 'substr' and 'strncmp'  
- suspicious condition (runtime comparison of string literals)  
- suspicious condition (string literals as boolean)  
- suspicious comparison of a string literal with a char* variable  
- suspicious comparison of '\0' with a char* variable  
- overlapping strcmp() expression  

**Type**  
Type checks  
- bitwise shift by too many bits (only enabled when --platform is used)  
- signed integer overflow (only enabled when --platform is used)  
- dangerous sign conversion, when signed value can be negative  
- possible loss of information when assigning int result to long variable  
- possible loss of information when returning int result as long return value  
- float conversion overflow  

**Uninitialized variables**    
Uninitialized variables  
- using uninitialized local variables  
- using allocated data before it has been initialized  
- using dead pointer  

**Unused functions**  
Check for functions that are never called  

**UnusedVar**  
UnusedVar checks  
- unused variable  
- allocated but unused variable  
- unred variable  
- unassigned variable  
- unused struct member  

**Using postfix operators**  
Warn if using postfix operators ++ or -- rather than prefix operator  

**Vaarg**  
Check for misusage of variable argument lists:  
- Wrong parameter passed to va_start()  
- Reference passed to va_start()  
- Missing va_end()  
- Using va_list before it is opened  
- Subsequent calls to va_start/va_copy()  

https://blog.csdn.net/u011430225/article/details/51497641

### 2.4 Detail Information （XML format）
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
    <cppcheck version="1.83"/>
    <errors>
		<error id="purgedConfiguration" severity="information" msg="The configuration &apos;&apos; was not checked because its code equals another one." verbose="The configuration &apos;&apos; was not checked because its code equals another one."/>
        <error id="toomanyconfigs" severity="information" msg="Too many #ifdef configurations - cppcheck only checks 12 configurations. Use --force to check all configurations. For more details, use --enable=information." verbose="The checking of the file will be interrupted because there are too many #ifdef configurations. Checking of all #ifdef configurations can be forced by --force command line option or from GUI preferences. However that may increase the checking time. For more details, use --enable=information." cwe="398"/>
        <error id="AssignmentAddressToInteger" severity="portability" msg="Assigning a pointer to an integer is not portable." verbose="Assigning a pointer to an integer (int/long/etc) is not portable across different platforms and compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux they are of different width. In worst case you end up assigning 64-bit address to 32-bit integer. The safe way is to store addresses only in pointer types (or typedefs like uintptr_t)." cwe="758"/>
        <error id="AssignmentIntegerToAddress" severity="portability" msg="Assigning an integer to a pointer is not portable." verbose="Assigning an integer (int/long/etc) to a pointer is not portable across different platforms and compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux they are of different width. In worst case you end up assigning 64-bit integer to 32-bit pointer. The safe way is to store addresses only in pointer types (or typedefs like uintptr_t)." cwe="758"/>
        <error id="CastIntegerToAddressAtReturn" severity="portability" msg="Returning an integer in a function with pointer return type is not portable." verbose="Returning an integer (int/long/etc) in a function with pointer return type is not portable across different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit integer down to 32-bit pointer. The safe way is to always return a pointer." cwe="758"/>
        <error id="CastAddressToIntegerAtReturn" severity="portability" msg="Returning an address value in a function with integer return type is not portable." verbose="Returning an address value in a function with integer (int/long/etc) return type is not portable across different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit address down to 32-bit integer. The safe way is to always return an integer." cwe="758"/>
        <error id="assertWithSideEffect" severity="warning" msg="Assert statement calls a function which may have desired side effects: &apos;function&apos;." verbose="Non-pure function: &apos;function&apos; is called inside assert statement. Assert statements are removed from release builds so the code inside assert statement is not executed. If the code is needed also in release builds, this is a bug." cwe="398"/>
        <error id="assignmentInAssert" severity="warning" msg="Assert statement modifies &apos;var&apos;." verbose="Variable &apos;var&apos; is modified insert assert statement. Assert statements are removed from release builds so the code inside assert statement is not executed. If the code is needed also in release builds, this is a bug." cwe="398"/>
        <error id="autoVariables" severity="error" msg="Address of local auto-variable assigned to a function parameter." verbose="Dangerous assignment - the function parameter is assigned the address of a local auto-variable. Local auto-variables are reserved from the stack which is freed when the function ends. So the pointer to a local variable is invalid after the function ends." cwe="562"/>
        <error id="returnAddressOfAutoVariable" severity="error" msg="Address of an auto-variable returned." verbose="Address of an auto-variable returned." cwe="562"/>
        <error id="autoVariablesAssignGlobalPointer" severity="warning" msg="Address of local array array is assigned to global pointer pointer and not reassigned before array goes out of scope." verbose="Address of local array array is assigned to global pointer pointer and not reassigned before array goes out of scope." cwe="562"/>
        <error id="returnLocalVariable" severity="error" msg="Pointer to local array variable returned." verbose="Pointer to local array variable returned." cwe="562"/>
        <error id="returnReference" severity="error" msg="Reference to auto variable returned." verbose="Reference to auto variable returned." cwe="562"/>
        <error id="returnTempReference" severity="error" msg="Reference to temporary returned." verbose="Reference to temporary returned." cwe="562"/>
        <error id="autovarInvalidDeallocation" severity="error" msg="Deallocation of an auto-variable results in undefined behaviour." verbose="The deallocation of an auto-variable results in undefined behaviour. You should only free memory that has been allocated dynamically." cwe="590"/>
        <error id="returnAddressOfFunctionParameter" severity="error" msg="Address of function parameter &apos;parameter&apos; returned." verbose="Address of the function parameter &apos;parameter&apos; becomes invalid after the function exits because function parameters are stored on the stack which is freed when the function exits. Thus the returned value is invalid." cwe="562"/>
        <error id="uselessAssignmentArg" severity="style" msg="Assignment of function parameter has no effect outside the function." verbose="Assignment of function parameter has no effect outside the function." cwe="398"/>
        <error id="uselessAssignmentPtrArg" severity="warning" msg="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" verbose="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" cwe="398"/>
        <error id="assignBoolToPointer" severity="error" msg="Boolean value assigned to pointer." verbose="Boolean value assigned to pointer." cwe="587"/>
        <error id="assignBoolToFloat" severity="style" msg="Boolean value assigned to floating point variable." verbose="Boolean value assigned to floating point variable." cwe="704"/>
        <error id="comparisonOfFuncReturningBoolError" severity="style" msg="Comparison of a function returning boolean value using relational (&lt;, &gt;, &lt;= or &gt;=) operator." verbose="The return type of function &apos;func_name&apos; is &apos;bool&apos; and result is of type &apos;bool&apos;. Comparing &apos;bool&apos; value using relational (&lt;, &gt;, &lt;= or &gt;=) operator could cause unexpected results." cwe="398"/>
        <error id="comparisonOfTwoFuncsReturningBoolError" severity="style" msg="Comparison of two functions returning boolean value using relational (&lt;, &gt;, &lt;= or &gt;=) operator." verbose="The return type of function &apos;func_name1&apos; and function &apos;func_name2&apos; is &apos;bool&apos; and result is of type &apos;bool&apos;. Comparing &apos;bool&apos; value using relational (&lt;, &gt;, &lt;= or &gt;=) operator could cause unexpected results." cwe="398"/>
        <error id="comparisonOfBoolWithBoolError" severity="style" msg="Comparison of a variable having boolean value using relational (&lt;, &gt;, &lt;= or &gt;=) operator." verbose="The variable &apos;var_name&apos; is of type &apos;bool&apos; and comparing &apos;bool&apos; value using relational (&lt;, &gt;, &lt;= or &gt;=) operator could cause unexpected results." cwe="398"/>
        <error id="incrementboolean" severity="style" msg="Incrementing a variable of type &apos;bool&apos; with postfix operator++ is deprecated by the C++ Standard. You should assign it the value &apos;true&apos; instead." verbose="The operand of a postfix increment operator may be of type bool but it is deprecated by C++ Standard (Annex D-1) and the operand is always set to true. You should assign it the value &apos;true&apos; instead." cwe="398"/>
        <error id="bitwiseOnBoolean" severity="style" msg="Boolean variable &apos;varname&apos; is used in bitwise operation. Did you mean &apos;&amp;&amp;&apos;?" verbose="Boolean variable &apos;varname&apos; is used in bitwise operation. Did you mean &apos;&amp;&amp;&apos;?" cwe="398" inconclusive="true"/>
        <error id="compareBoolExpressionWithInt" severity="warning" msg="Comparison of a boolean expression with an integer other than 0 or 1." verbose="Comparison of a boolean expression with an integer other than 0 or 1." cwe="398"/>
        <error id="pointerArithBool" severity="error" msg="Converting pointer arithmetic result to bool. The bool is always true unless there is undefined behaviour." verbose="Converting pointer arithmetic result to bool. The boolean result is always true unless there is pointer arithmetic overflow, and overflow is undefined behaviour. Probably a dereference is forgotten." cwe="571"/>
        <error id="boostForeachError" severity="error" msg="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." verbose="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." cwe="664"/>
        <error id="arrayIndexOutOfBounds" severity="error" msg="Array &apos;array[2]&apos; index array[1][1] out of bounds." verbose="Array &apos;array[2]&apos; index array[1][1] out of bounds." cwe="788"/>
        <error id="bufferAccessOutOfBounds" severity="error" msg="Buffer is accessed out of bounds: buffer" verbose="Buffer is accessed out of bounds: buffer" cwe="788"/>
        <error id="strncatUsage" severity="warning" msg="Dangerous usage of strncat - 3rd parameter is the maximum number of characters to append." verbose="At most, strncat appends the 3rd parameter&apos;s amount of characters and adds a terminating null byte.\012The safe way to use strncat is to subtract one from the remaining space in the buffer and use it as 3rd parameter.Source: http://www.cplusplus.com/reference/cstring/strncat/\012Source: http://www.opensource.apple.com/source/Libc/Libc-167/gen.subproj/i386.subproj/strncat.c" cwe="119"/>
        <error id="outOfBounds" severity="error" msg="index is out of bounds: Supplied size 2 is larger than actual size 1." verbose="index is out of bounds: Supplied size 2 is larger than actual size 1." cwe="788"/>
        <error id="sizeArgumentAsChar" severity="warning" msg="The size argument is given as a char constant." verbose="The size argument is given as a char constant." cwe="682"/>
        <error id="terminateStrncpy" severity="warning" msg="The buffer &apos;buffer&apos; may not be null-terminated after the call to strncpy()." verbose="If the source string&apos;s size fits or exceeds the given size, strncpy() does not add a zero at the end of the buffer. This causes bugs later in the code if the code assumes buffer is null-terminated." cwe="170" inconclusive="true"/>
        <error id="bufferNotZeroTerminated" severity="warning" msg="The buffer &apos;buffer&apos; is not null-terminated after the call to strncpy()." verbose="The buffer &apos;buffer&apos; is not null-terminated after the call to strncpy(). This will cause bugs later in the code if the code assumes the buffer is null-terminated." cwe="170" inconclusive="true"/>
        <error id="negativeIndex" severity="error" msg="Array index -1 is out of bounds." verbose="Array index -1 is out of bounds." cwe="786"/>
        <error id="insecureCmdLineArgs" severity="error" msg="Buffer overrun possible for long command line arguments." verbose="Buffer overrun possible for long command line arguments." cwe="119"/>
        <error id="pointerOutOfBounds" severity="portability" msg="Undefined behaviour, pointer arithmetic &apos;&apos; is out of bounds." verbose="Undefined behaviour, pointer arithmetic &apos;&apos; is out of bounds. From chapter 6.5.6 in the C specification:\012&quot;When an expression that has integer type is added to or subtracted from a pointer, ..&quot; and then &quot;If both the pointer operand and the result point to elements of the same array object, or one past the last element of the array object, the evaluation shall not produce an overflow; otherwise, the behavior is undefined.&quot;" cwe="398"/>
        <error id="arrayIndexThenCheck" severity="style" msg="Array index &apos;index&apos; is used before limits check." verbose="Defensive programming: The variable &apos;index&apos; is used as an array index before it is checked that is within limits. This can mean that the array might be accessed out of bounds. Reorder conditions such as &apos;(a[i] &amp;&amp; i &lt; 10)&apos; to &apos;(i &lt; 10 &amp;&amp; a[i])&apos;. That way the array will not be accessed if the index is out of limits." cwe="398"/>
        <error id="possibleBufferAccessOutOfBounds" severity="warning" msg="Possible buffer overflow if strlen(source) is larger than or equal to sizeof(destination)." verbose="Possible buffer overflow if strlen(source) is larger than or equal to sizeof(destination). The source buffer is larger than the destination buffer so there is the potential for overflowing the destination buffer." cwe="398"/>
        <error id="argumentSize" severity="warning" msg="The array &apos;array&apos; is too small, the function &apos;function&apos; expects a bigger one." verbose="The array &apos;array&apos; is too small, the function &apos;function&apos; expects a bigger one." cwe="398"/>
        <error id="negativeMemoryAllocationSize" severity="error" msg="Memory allocation size is negative." verbose="Memory allocation size is negative.Negative allocation size has no specified behaviour." cwe="131"/>
        <error id="negativeArraySize" severity="error" msg="Declaration of array &apos;&apos; with negative size is undefined behaviour" verbose="Declaration of array &apos;&apos; with negative size is undefined behaviour" cwe="758"/>
        <error id="arrayIndexOutOfBoundsCond" severity="warning" msg="Array &apos;x[10]&apos; accessed at index 20, which is out of bounds. Otherwise condition &apos;y==20&apos; is redundant." verbose="Array &apos;x[10]&apos; accessed at index 20, which is out of bounds. Otherwise condition &apos;y==20&apos; is redundant." cwe="119"/>
        <error id="invalidFunctionArg" severity="error" msg="Invalid func_name() argument nr 1. The value is 0 or 1 (boolean) but the valid values are &apos;1:4&apos;." verbose="Invalid func_name() argument nr 1. The value is 0 or 1 (boolean) but the valid values are &apos;1:4&apos;." cwe="628"/>
        <error id="invalidFunctionArgBool" severity="error" msg="Invalid func_name() argument nr 1. A non-boolean value is required." verbose="Invalid func_name() argument nr 1. A non-boolean value is required." cwe="628"/>
        <error id="ignoredReturnValue" severity="warning" msg="Return value of function malloc() is not used." verbose="Return value of function malloc() is not used." cwe="252"/>
        <error id="wrongmathcall" severity="warning" msg="Passing value &apos;#&apos; to #() leads to implementation-defined result." verbose="Passing value &apos;#&apos; to #() leads to implementation-defined result." cwe="758"/>
        <error id="unpreciseMathCall" severity="style" msg="Expression &apos;1 - erf(x)&apos; can be replaced by &apos;erfc(x)&apos; to avoid loss of precision." verbose="Expression &apos;1 - erf(x)&apos; can be replaced by &apos;erfc(x)&apos; to avoid loss of precision." cwe="758"/>
        <error id="memsetZeroBytes" severity="warning" msg="memset() called to fill 0 bytes." verbose="memset() called to fill 0 bytes. The second and third arguments might be inverted. The function memset ( void * ptr, int value, size_t num ) sets the first num bytes of the block of memory pointed by ptr to the specified value." cwe="687"/>
        <error id="memsetFloat" severity="portability" msg="The 2nd memset() argument &apos;varname&apos; is a float, its representation is implementation defined." verbose="The 2nd memset() argument &apos;varname&apos; is a float, its representation is implementation defined. memset() is used to set each byte of a block of memory to a specific value and the actual representation of a floating-point value is implementation defined." cwe="688"/>
        <error id="memsetValueOutOfRange" severity="warning" msg="The 2nd memset() argument &apos;varname&apos; doesn&apos;t fit into an &apos;unsigned char&apos;." verbose="The 2nd memset() argument &apos;varname&apos; doesn&apos;t fit into an &apos;unsigned char&apos;. The 2nd parameter is passed as an &apos;int&apos;, but the function fills the block of memory using the &apos;unsigned char&apos; conversion of this value." cwe="686"/>
        <error id="noConstructor" severity="style" msg="The class &apos;classname&apos; does not have a constructor." verbose="The class &apos;classname&apos; does not have a constructor although it has private member variables. Member variables of builtin types are left uninitialized when the class is instantiated. That may cause bugs or undefined behavior." cwe="398"/>
        <error id="noExplicitConstructor" severity="style" msg="Class &apos;classname&apos; has a constructor with 1 argument that is not explicit." verbose="Class &apos;classname&apos; has a constructor with 1 argument that is not explicit. Such constructors should in general be explicit for type safety reasons. Using the explicit keyword in the constructor means some mistakes when using the class can be avoided." cwe="398"/>
        <error id="copyCtorPointerCopying" severity="style" msg="Value of pointer &apos;var&apos;, which points to allocated memory, is copied in copy constructor instead of allocating new memory." verbose="Value of pointer &apos;var&apos;, which points to allocated memory, is copied in copy constructor instead of allocating new memory." cwe="398"/>
        <error id="noCopyConstructor" severity="style" msg="class &apos;class&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." verbose="class &apos;class&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." cwe="398"/>
        <error id="uninitMemberVar" severity="warning" msg="Member variable &apos;classname::varname&apos; is not initialized in the constructor." verbose="Member variable &apos;classname::varname&apos; is not initialized in the constructor." cwe="398"/>
        <error id="operatorEqVarError" severity="warning" msg="Member variable &apos;classname::&apos; is not assigned a value in &apos;classname::operator=&apos;." verbose="Member variable &apos;classname::&apos; is not assigned a value in &apos;classname::operator=&apos;." cwe="398"/>
        <error id="unusedPrivateFunction" severity="style" msg="Unused private function: &apos;classname::funcname&apos;" verbose="Unused private function: &apos;classname::funcname&apos;" cwe="398"/>
        <error id="memsetClass" severity="error" msg="Using &apos;memfunc&apos; on class that contains a classname." verbose="Using &apos;memfunc&apos; on class that contains a classname is unsafe, because constructor, destructor and copy operator calls are omitted. These are necessary for this non-POD type to ensure that a valid object is created." cwe="762"/>
        <error id="memsetClassReference" severity="error" msg="Using &apos;memfunc&apos; on class that contains a reference." verbose="Using &apos;memfunc&apos; on class that contains a reference." cwe="665"/>
        <error id="memsetClassFloat" severity="portability" msg="Using memset() on class which contains a floating point number." verbose="Using memset() on class which contains a floating point number. This is not portable because memset() sets each byte of a block of memory to a specific value and the actual representation of a floating-point value is implementation defined. Note: In case of an IEEE754-1985 compatible implementation setting all bits to zero results in the value 0.0." cwe="758"/>
        <error id="mallocOnClassWarning" severity="warning" msg="Memory for class instance allocated with malloc(), but class provides constructors." verbose="Memory for class instance allocated with malloc(), but class provides constructors. This is unsafe, since no constructor is called and class members remain uninitialized. Consider using &apos;new&apos; instead." cwe="762"/>
        <error id="mallocOnClassError" severity="error" msg="Memory for class instance allocated with malloc(), but class contains a std::string." verbose="Memory for class instance allocated with malloc(), but class a std::string. This is unsafe, since no constructor is called and class members remain uninitialized. Consider using &apos;new&apos; instead." cwe="665"/>
        <error id="operatorEq" severity="style" msg="&apos;class::operator=&apos; should return &apos;class &amp;&apos;." verbose="The class::operator= does not conform to standard C/C++ behaviour. To conform to standard C/C++ behaviour, return a reference to self (such as: &apos;class &amp;class::operator=(..) { .. return *this; }&apos;. For safety reasons it might be better to not fix this message. If you think that safety is always more important than conformance then please ignore/suppress this message. For more details about this topic, see the book &quot;Effective C++&quot; by Scott Meyers." cwe="398"/>
        <error id="virtualDestructor" severity="error" msg="Class &apos;Base&apos; which is inherited by class &apos;Derived&apos; does not have a virtual destructor." verbose="Class &apos;Base&apos; which is inherited by class &apos;Derived&apos; does not have a virtual destructor. If you destroy instances of the derived class by deleting a pointer that points to the base class, only the destructor of the base class is executed. Thus, dynamic memory that is managed by the derived class could leak. This can be avoided by adding a virtual destructor to the base class." cwe="404"/>
        <error id="thisSubtraction" severity="warning" msg="Suspicious pointer subtraction. Did you intend to write &apos;-&gt;&apos;?" verbose="Suspicious pointer subtraction. Did you intend to write &apos;-&gt;&apos;?" cwe="398"/>
        <error id="operatorEqRetRefThis" severity="style" msg="&apos;operator=&apos; should return reference to &apos;this&apos; instance." verbose="&apos;operator=&apos; should return reference to &apos;this&apos; instance." cwe="398"/>
        <error id="operatorEqMissingReturnStatement" severity="error" msg="No &apos;return&apos; statement in non-void function causes undefined behavior." verbose="No &apos;return&apos; statement in non-void function causes undefined behavior." cwe="398"/>
        <error id="operatorEqShouldBeLeftUnimplemented" severity="style" msg="&apos;operator=&apos; should either return reference to &apos;this&apos; instance or be declared private and left unimplemented." verbose="&apos;operator=&apos; should either return reference to &apos;this&apos; instance or be declared private and left unimplemented." cwe="398"/>
        <error id="operatorEqToSelf" severity="warning" msg="&apos;operator=&apos; should check for assignment to self to avoid problems with dynamic memory." verbose="&apos;operator=&apos; should check for assignment to self to ensure that each block of dynamically allocated memory is owned and managed by only one instance of the class." cwe="398"/>
        <error id="functionConst" severity="style" msg="Technically the member function &apos;class::function&apos; can be const." verbose="The member function &apos;class::function&apos; can be made a const function. Making this function &apos;const&apos; should not cause compiler errors. Even though the function can be made const function technically it may not make sense conceptually. Think about your design and the task of the function first - is it a function that must not change object internal state?" cwe="398" inconclusive="true"/>
        <error id="functionStatic" severity="performance" msg="Technically the member function &apos;class::function&apos; can be static." verbose="The member function &apos;class::function&apos; can be made a static function. Making a function static can bring a performance benefit since no &apos;this&apos; instance is passed to the function. This change should not cause compiler errors but it does not necessarily make sense conceptually. Think about your design and the task of the function first - is it a function that must not access members of class instances?" cwe="398" inconclusive="true"/>
        <error id="initializerList" severity="style" msg="Member variable &apos;class::variable&apos; is in the wrong place in the initializer list." verbose="Member variable &apos;class::variable&apos; is in the wrong place in the initializer list. Members are initialized in the order they are declared, not in the order they are in the initializer list.  Keeping the initializer list in the same order that the members were declared prevents order dependent initialization errors." cwe="398" inconclusive="true"/>
        <error id="useInitializationList" severity="performance" msg="Variable &apos;variable&apos; is assigned in constructor body. Consider performing initialization in initialization list." verbose="When an object of a class is created, the constructors of all member variables are called consecutively in the order the variables are declared, even if you don&apos;t explicitly write them to the initialization list. You could avoid assigning &apos;variable&apos; a value by passing the value to the constructor in the initialization list." cwe="398"/>
        <error id="selfInitialization" severity="error" msg="Member variable &apos;var&apos; is initialized by itself." verbose="Member variable &apos;var&apos; is initialized by itself." cwe="665"/>
        <error id="duplInheritedMember" severity="warning" msg="The class &apos;class&apos; defines member variable with name &apos;variable&apos; also defined in its parent class &apos;class&apos;." verbose="The class &apos;class&apos; defines member variable with name &apos;variable&apos; also defined in its parent class &apos;class&apos;." cwe="398"/>
        <error id="copyCtorAndEqOperator" severity="warning" msg="The class &apos;class&apos; has &apos;operator=&apos; but lack of &apos;copy constructor&apos;." verbose="The class &apos;class&apos; has &apos;operator=&apos; but lack of &apos;copy constructor&apos;."/>
        <error id="unsafeClassDivZero" severity="style" msg="Public interface of Class is not safe. When calling Class::dostuff(), if parameter x is 0 that leads to division by zero." verbose="Public interface of Class is not safe. When calling Class::dostuff(), if parameter x is 0 that leads to division by zero."/>
        <error id="assignIfError" severity="style" msg="Mismatching assignment and comparison, comparison &apos;&apos; is always false." verbose="Mismatching assignment and comparison, comparison &apos;&apos; is always false." cwe="398"/>
        <error id="badBitmaskCheck" severity="warning" msg="Result of operator &apos;|&apos; is always true if one operand is non-zero. Did you intend to use &apos;&amp;&apos;?" verbose="Result of operator &apos;|&apos; is always true if one operand is non-zero. Did you intend to use &apos;&amp;&apos;?" cwe="571"/>
        <error id="comparisonError" severity="style" msg="Expression &apos;(X &amp; 0x6) == 0x1&apos; is always false." verbose="The expression &apos;(X &amp; 0x6) == 0x1&apos; is always false. Check carefully constants and operators used, these errors might be hard to spot sometimes. In case of complex expression it might help to split it to separate expressions." cwe="398"/>
        <error id="multiCondition" severity="style" msg="Expression is always false because &apos;else if&apos; condition matches previous condition at line 1." verbose="Expression is always false because &apos;else if&apos; condition matches previous condition at line 1." cwe="398"/>
        <error id="mismatchingBitAnd" severity="style" msg="Mismatching bitmasks. Result is always 0 (X = Y &amp; 0xf0; Z = X &amp; 0x1; =&gt; Z=0)." verbose="Mismatching bitmasks. Result is always 0 (X = Y &amp; 0xf0; Z = X &amp; 0x1; =&gt; Z=0)." cwe="398"/>
        <error id="oppositeInnerCondition" severity="warning" msg="Opposite inner &apos;if&apos; condition leads to a dead code block." verbose="Opposite inner &apos;if&apos; condition leads to a dead code block (outer condition is &apos;x&apos; and inner condition is &apos;!x&apos;)." cwe="398"/>
        <error id="identicalConditionAfterEarlyExit" severity="warning" msg="Identical condition &apos;x&apos;, second condition is always false" verbose="Identical condition &apos;x&apos;, second condition is always false" cwe="398"/>
        <error id="incorrectLogicOperator" severity="warning" msg="Logical disjunction always evaluates to true: foo &gt; 3 &amp;&amp; foo &lt; 4." verbose="Logical disjunction always evaluates to true: foo &gt; 3 &amp;&amp; foo &lt; 4. Are these conditions necessary? Did you intend to use &amp;&amp; instead? Are the numbers correct? Are you comparing the correct variables?" cwe="571"/>
        <error id="redundantCondition" severity="style" msg="Redundant condition: If x &gt; 11 the condition x &gt; 10 is always true." verbose="Redundant condition: If x &gt; 11 the condition x &gt; 10 is always true." cwe="398"/>
        <error id="moduloAlwaysTrueFalse" severity="warning" msg="Comparison of modulo result is predetermined, because it is always less than 1." verbose="Comparison of modulo result is predetermined, because it is always less than 1." cwe="398"/>
        <error id="clarifyCondition" severity="style" msg="Suspicious condition (assignment + comparison); Clarify expression with parentheses." verbose="Suspicious condition (assignment + comparison); Clarify expression with parentheses." cwe="398"/>
        <error id="knownConditionTrueFalse" severity="style" msg="Condition &apos;x&apos; is always false" verbose="Condition &apos;x&apos; is always false" cwe="570"/>
        <error id="invalidTestForOverflow" severity="warning" msg="Invalid test for overflow &apos;x + u &lt; x&apos;. Condition is always false unless there is overflow, and overflow is undefined behaviour." verbose="Invalid test for overflow &apos;x + u &lt; x&apos;. Condition is always false unless there is overflow, and overflow is undefined behaviour." cwe="570"/>
        <error id="pointerAdditionResultNotNull" severity="warning" msg="Comparison is wrong. Result of &apos;ptr+1&apos; can&apos;t be 0 unless there is pointer overflow, and pointer overflow is undefined behaviour." verbose="Comparison is wrong. Result of &apos;ptr+1&apos; can&apos;t be 0 unless there is pointer overflow, and pointer overflow is undefined behaviour."/>
        <error id="exceptThrowInDestructor" severity="warning" msg="Class Class is not safe, destructor throws exception" verbose="The class Class is not safe because its destructor throws an exception. If Class is used and an exception is thrown that is caught in an outer scope the program will terminate." cwe="398"/>
        <error id="exceptDeallocThrow" severity="warning" msg="Exception thrown in invalid state, &apos;p&apos; points at deallocated memory." verbose="Exception thrown in invalid state, &apos;p&apos; points at deallocated memory." cwe="398"/>
        <error id="exceptRethrowCopy" severity="style" msg="Throwing a copy of the caught exception instead of rethrowing the original exception." verbose="Rethrowing an exception with &apos;throw varname;&apos; creates an unnecessary copy of &apos;varname&apos;. To rethrow the caught exception without unnecessary copying or slicing, use a bare &apos;throw;&apos;." cwe="398"/>
        <error id="catchExceptionByValue" severity="style" msg="Exception should be caught by reference." verbose="The exception is caught by value. It could be caught as a (const) reference which is usually recommended in C++." cwe="398"/>
        <error id="throwInNoexceptFunction" severity="error" msg="Exception thrown in function declared not to throw exceptions." verbose="Exception thrown in function declared not to throw exceptions." cwe="398"/>
        <error id="unhandledExceptionSpecification" severity="style" msg="Unhandled exception specification when calling function foo()." verbose="Unhandled exception specification when calling function foo(). Either use a try/catch around the function call, or add a exception specification for funcname() also." cwe="703" inconclusive="true"/>
        <error id="coutCerrMisusage" severity="error" msg="Invalid usage of output stream: &apos;&lt;&lt; std::cout&apos;." verbose="Invalid usage of output stream: &apos;&lt;&lt; std::cout&apos;." cwe="398"/>
        <error id="fflushOnInputStream" severity="portability" msg="fflush() called on input stream &apos;stdin&apos; may result in undefined behaviour on non-linux systems." verbose="fflush() called on input stream &apos;stdin&apos; may result in undefined behaviour on non-linux systems." cwe="398"/>
        <error id="IOWithoutPositioning" severity="error" msg="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." verbose="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." cwe="664"/>
        <error id="readWriteOnlyFile" severity="error" msg="Read operation on a file that was opened only for writing." verbose="Read operation on a file that was opened only for writing." cwe="664"/>
        <error id="writeReadOnlyFile" severity="error" msg="Write operation on a file that was opened only for reading." verbose="Write operation on a file that was opened only for reading." cwe="664"/>
        <error id="useClosedFile" severity="error" msg="Used file that is not opened." verbose="Used file that is not opened." cwe="910"/>
        <error id="seekOnAppendedFile" severity="warning" msg="Repositioning operation performed on a file opened in append mode has no effect." verbose="Repositioning operation performed on a file opened in append mode has no effect." cwe="398"/>
        <error id="invalidscanf" severity="warning" msg="scanf() without field width limits can crash with huge input data." verbose="scanf() without field width limits can crash with huge input data. Add a field width specifier to fix this problem.\012\012Sample program that can crash:\012\012#include &lt;stdio.h&gt;\012int main()\012{\012    char c[5];\012    scanf(&quot;%s&quot;, c);\012    return 0;\012}\012\012Typing in 5 or more characters may make the program crash. The correct usage here is &apos;scanf(&quot;%4s&quot;, c);&apos;, as the maximum field width does not include the terminating null byte.\012Source: http://linux.die.net/man/3/scanf\012Source: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/libkern/stdio/scanf.c" cwe="119"/>
        <error id="wrongPrintfScanfArgNum" severity="error" msg="printf format string requires 3 parameters but only 2 are given." verbose="printf format string requires 3 parameters but only 2 are given." cwe="685"/>
        <error id="invalidScanfArgType_s" severity="warning" msg="%s in format string (no. 1) requires a &apos;char *&apos; but the argument type is Unknown." verbose="%s in format string (no. 1) requires a &apos;char *&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidScanfArgType_int" severity="warning" msg="%d in format string (no. 1) requires &apos;int *&apos; but the argument type is Unknown." verbose="%d in format string (no. 1) requires &apos;int *&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidScanfArgType_float" severity="warning" msg="%f in format string (no. 1) requires &apos;float *&apos; but the argument type is Unknown." verbose="%f in format string (no. 1) requires &apos;float *&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_s" severity="warning" msg="%s in format string (no. 1) requires &apos;char *&apos; but the argument type is Unknown." verbose="%s in format string (no. 1) requires &apos;char *&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_n" severity="warning" msg="%n in format string (no. 1) requires &apos;int *&apos; but the argument type is Unknown." verbose="%n in format string (no. 1) requires &apos;int *&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_p" severity="warning" msg="%p in format string (no. 1) requires an address but the argument type is Unknown." verbose="%p in format string (no. 1) requires an address but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_uint" severity="warning" msg="%u in format string (no. 1) requires &apos;unsigned int&apos; but the argument type is Unknown." verbose="%u in format string (no. 1) requires &apos;unsigned int&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_sint" severity="warning" msg="%i in format string (no. 1) requires &apos;int&apos; but the argument type is Unknown." verbose="%i in format string (no. 1) requires &apos;int&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidPrintfArgType_float" severity="warning" msg="%f in format string (no. 1) requires &apos;double&apos; but the argument type is Unknown." verbose="%f in format string (no. 1) requires &apos;double&apos; but the argument type is Unknown." cwe="686"/>
        <error id="invalidLengthModifierError" severity="warning" msg="&apos;I&apos; in format string (no. 1) is a length modifier and cannot be used without a conversion specifier." verbose="&apos;I&apos; in format string (no. 1) is a length modifier and cannot be used without a conversion specifier." cwe="704"/>
        <error id="invalidScanfFormatWidth" severity="error" msg="Width 5 given in format string (no. 10) is larger than destination buffer &apos;[0]&apos;, use %-1s to prevent overflowing it." verbose="Width 5 given in format string (no. 10) is larger than destination buffer &apos;[0]&apos;, use %-1s to prevent overflowing it." cwe="687"/>
        <error id="wrongPrintfScanfParameterPositionError" severity="warning" msg="printf: referencing parameter 2 while 1 arguments given" verbose="printf: referencing parameter 2 while 1 arguments given" cwe="685"/>
        <error id="deallocret" severity="error" msg="Returning/dereferencing &apos;p&apos; after it is deallocated / released" verbose="Returning/dereferencing &apos;p&apos; after it is deallocated / released" cwe="672"/>
        <error id="doubleFree" severity="error" msg="Memory pointed to by &apos;varname&apos; is freed twice." verbose="Memory pointed to by &apos;varname&apos; is freed twice." cwe="415"/>
        <error id="leakNoVarFunctionCall" severity="error" msg="Allocation with funcName, funcName doesn&apos;t release it." verbose="Allocation with funcName, funcName doesn&apos;t release it." cwe="772"/>
        <error id="leakReturnValNotUsed" severity="error" msg="Return value of allocation function &apos;funcName&apos; is not stored." verbose="Return value of allocation function &apos;funcName&apos; is not stored." cwe="771"/>
        <error id="leakUnsafeArgAlloc" severity="warning" msg="Unsafe allocation. If funcName() throws, memory could be leaked. Use make_shared&lt;int&gt;() instead." verbose="Unsafe allocation. If funcName() throws, memory could be leaked. Use make_shared&lt;int&gt;() instead." cwe="401" inconclusive="true"/>
        <error id="publicAllocationError" severity="warning" msg="Possible leak in public function. The pointer &apos;varname&apos; is not deallocated before it is allocated." verbose="Possible leak in public function. The pointer &apos;varname&apos; is not deallocated before it is allocated." cwe="398"/>
        <error id="unsafeClassCanLeak" severity="style" msg="Class &apos;class&apos; is unsafe, &apos;class::varname&apos; can leak by wrong usage." verbose="The class &apos;class&apos; is unsafe, wrong usage can cause memory/resource leaks for &apos;class::varname&apos;. This can for instance be fixed by adding proper cleanup in the destructor." cwe="398"/>
        <error id="memleak" severity="error" msg="Memory leak: varname" verbose="Memory leak: varname" cwe="401"/>
        <error id="resourceLeak" severity="error" msg="Resource leak: varname" verbose="Resource leak: varname" cwe="775"/>
        <error id="deallocDealloc" severity="error" msg="Deallocating a deallocated pointer: varname" verbose="Deallocating a deallocated pointer: varname" cwe="415"/>
        <error id="deallocuse" severity="error" msg="Dereferencing &apos;varname&apos; after it is deallocated / released" verbose="Dereferencing &apos;varname&apos; after it is deallocated / released" cwe="416"/>
        <error id="mismatchSize" severity="error" msg="The allocated size sz is not a multiple of the underlying type&apos;s size." verbose="The allocated size sz is not a multiple of the underlying type&apos;s size." cwe="131"/>
        <error id="mismatchAllocDealloc" severity="error" msg="Mismatching allocation and deallocation: varname" verbose="Mismatching allocation and deallocation: varname" cwe="762"/>
        <error id="memleakOnRealloc" severity="error" msg="Common realloc mistake: &apos;varname&apos; nulled but not freed upon failure" verbose="Common realloc mistake: &apos;varname&apos; nulled but not freed upon failure" cwe="401"/>
        <error id="nullPointer" severity="error" msg="Null pointer dereference" verbose="Null pointer dereference" cwe="476"/>
        <error id="nullPointerDefaultArg" severity="warning" msg="Possible null pointer dereference if the default parameter value is used: pointer" verbose="Possible null pointer dereference if the default parameter value is used: pointer" cwe="476"/>
        <error id="nullPointerRedundantCheck" severity="warning" msg="Either the condition is redundant or there is possible null pointer dereference: pointer." verbose="Either the condition is redundant or there is possible null pointer dereference: pointer." cwe="476"/>
        <error id="nullPointerArithmetic" severity="error" msg="Overflow in pointer arithmetic, NULL pointer is subtracted." verbose="Overflow in pointer arithmetic, NULL pointer is subtracted." cwe="682"/>
        <error id="zerodiv" severity="error" msg="Division by zero." verbose="Division by zero." cwe="369"/>
        <error id="zerodivcond" severity="error" msg="Either the condition is redundant or there is division by zero." verbose="Either the condition is redundant or there is division by zero." cwe="369"/>
        <error id="unusedScopedObject" severity="style" msg="Instance of &apos;varname&apos; object is destroyed immediately." verbose="Instance of &apos;varname&apos; object is destroyed immediately." cwe="563"/>
        <error id="invalidPointerCast" severity="portability" msg="Casting between float* and double* which have an incompatible binary data representation." verbose="Casting between float* and double* which have an incompatible binary data representation." cwe="704"/>
        <error id="shiftNegativeLHS" severity="portability" msg="Shifting a negative value is technically undefined behaviour" verbose="Shifting a negative value is technically undefined behaviour" cwe="758"/>
        <error id="shiftNegative" severity="error" msg="Shifting by a negative value is undefined behaviour" verbose="Shifting by a negative value is undefined behaviour" cwe="758"/>
        <error id="wrongPipeParameterSize" severity="error" msg="Buffer &apos;varname&apos; must have size of 2 integers if used as parameter of pipe()." verbose="The pipe()/pipe2() system command takes an argument, which is an array of exactly two integers.\012The variable &apos;varname&apos; is an array of size dimension, which does not match." cwe="686"/>
        <error id="raceAfterInterlockedDecrement" severity="error" msg="Race condition: non-interlocked access after InterlockedDecrement(). Use InterlockedDecrement() return value instead." verbose="Race condition: non-interlocked access after InterlockedDecrement(). Use InterlockedDecrement() return value instead." cwe="362"/>
        <error id="redundantCopyLocalConst" severity="performance" msg="Use const reference for &apos;varname&apos; to avoid unnecessary data copying." verbose="The const variable &apos;varname&apos; is assigned a copy of the data. You can avoid the unnecessary data copying by converting &apos;varname&apos; to const reference." cwe="398" inconclusive="true"/>
        <error id="redundantCopy" severity="performance" msg="Buffer &apos;var&apos; is being written before its old content has been used." verbose="Buffer &apos;var&apos; is being written before its old content has been used." cwe="563"/>
        <error id="redundantAssignment" severity="style" msg="Variable &apos;var&apos; is reassigned a value before the old one has been used." verbose="Variable &apos;var&apos; is reassigned a value before the old one has been used." cwe="563"/>
        <error id="comparisonFunctionIsAlwaysTrueOrFalse" severity="warning" msg="Comparison of two identical variables with isless(varName,varName) always evaluates to false." verbose="The function isless is designed to compare two variables. Calling this function with one variable (varName) for both parameters leads to a statement which is always false." cwe="570"/>
        <error id="checkCastIntToCharAndBack" severity="warning" msg="Storing func_name() return value in char variable and then comparing with EOF." verbose="When saving func_name() return value in char variable there is loss of precision.  When func_name() returns EOF this value is truncated. Comparing the char variable with EOF can have unexpected results. For instance a loop &quot;while (EOF != (c = func_name());&quot; loops forever on some compilers/platforms and on other compilers/platforms it will stop when the file contains a matching character." cwe="197"/>
        <error id="cstyleCast" severity="style" msg="C-style pointer casting" verbose="C-style pointer casting detected. C++ offers four different kinds of casts as replacements: static_cast, const_cast, dynamic_cast and reinterpret_cast. A C-style cast could evaluate to any of those automatically, thus it is considered safer if the programmer explicitly states which kind of cast is expected. See also: https://www.securecoding.cert.org/confluence/display/cplusplus/EXP05-CPP.+Do+not+use+C-style+casts." cwe="398"/>
        <error id="passedByValue" severity="performance" msg="Function parameter &apos;parametername&apos; should be passed by reference." verbose="Parameter &apos;parametername&apos; is passed by value. It could be passed as a (const) reference which is usually faster and recommended in C++." cwe="398"/>
        <error id="constStatement" severity="warning" msg="Redundant code: Found a statement that begins with type constant." verbose="Redundant code: Found a statement that begins with type constant." cwe="398"/>
        <error id="signedCharArrayIndex" severity="warning" msg="Signed &apos;char&apos; type used as array index." verbose="Signed &apos;char&apos; type used as array index. If the value can be greater than 127 there will be a buffer underflow because of sign extension." cwe="128"/>
        <error id="unknownSignCharArrayIndex" severity="portability" msg="&apos;char&apos; type used as array index." verbose="&apos;char&apos; type used as array index. Values greater that 127 will be treated depending on whether &apos;char&apos; is signed or unsigned on target platform." cwe="758"/>
        <error id="charBitOp" severity="warning" msg="When using &apos;char&apos; variables in bit operations, sign extension can generate unexpected results." verbose="When using &apos;char&apos; variables in bit operations, sign extension can generate unexpected results. For example:\012    char c = 0x80;\012    int i = 0 | c;\012    if (i &amp; 0x8000)\012        printf(&quot;not expected&quot;);\012The &quot;not expected&quot; will be printed on the screen." cwe="398"/>
        <error id="variableScope" severity="style" msg="The scope of the variable &apos;varname&apos; can be reduced." verbose="The scope of the variable &apos;varname&apos; can be reduced. Warning: Be careful when fixing this message, especially when there are inner loops. Here is an example where cppcheck will write that the scope for &apos;i&apos; can be reduced:\012void f(int x)\012{\012    int i = 0;\012    if (x) {\012        // it&apos;s safe to move &apos;int i = 0;&apos; here\012        for (int n = 0; n &lt; 10; ++n) {\012            // it is possible but not safe to move &apos;int i = 0;&apos; here\012            do_something(&amp;i);\012        }\012    }\012}\012When you see this message it is always safe to reduce the variable scope 1 level." cwe="398"/>
        <error id="redundantAssignInSwitch" severity="warning" msg="Variable &apos;var&apos; is reassigned a value before the old one has been used. &apos;break;&apos; missing?" verbose="Variable &apos;var&apos; is reassigned a value before the old one has been used. &apos;break;&apos; missing?" cwe="563"/>
        <error id="redundantCopyInSwitch" severity="warning" msg="Buffer &apos;var&apos; is being written before its old content has been used. &apos;break;&apos; missing?" verbose="Buffer &apos;var&apos; is being written before its old content has been used. &apos;break;&apos; missing?" cwe="563"/>
        <error id="suspiciousCase" severity="warning" msg="Found suspicious case label in switch(). Operator &apos;||&apos; probably doesn&apos;t work as intended." verbose="Using an operator like &apos;||&apos; in a case label is suspicious. Did you intend to use a bitwise operator, multiple case labels or if/else instead?" cwe="398" inconclusive="true"/>
        <error id="suspiciousEqualityComparison" severity="warning" msg="Found suspicious equality comparison. Did you intend to assign a value instead?" verbose="Found suspicious equality comparison. Did you intend to assign a value instead?" cwe="482" inconclusive="true"/>
        <error id="selfAssignment" severity="warning" msg="Redundant assignment of &apos;varname&apos; to itself." verbose="Redundant assignment of &apos;varname&apos; to itself." cwe="398"/>
        <error id="clarifyCalculation" severity="style" msg="Clarify calculation precedence for &apos;+&apos; and &apos;?&apos;." verbose="Suspicious calculation. Please use parentheses to clarify the code. The code &apos;&apos;a+b?c:d&apos;&apos; should be written as either &apos;&apos;(a+b)?c:d&apos;&apos; or &apos;&apos;a+(b?c:d)&apos;&apos;." cwe="783"/>
        <error id="clarifyStatement" severity="warning" msg="Ineffective statement similar to &apos;*A++;&apos;. Did you intend to write &apos;(*A)++;&apos;?" verbose="A statement like &apos;*A++;&apos; might not do what you intended. Postfix &apos;operator++&apos; is executed before &apos;operator*&apos;. Thus, the dereference is meaningless. Did you intend to write &apos;(*A)++;&apos;?" cwe="783"/>
        <error id="duplicateBranch" severity="style" msg="Found duplicate branches for &apos;if&apos; and &apos;else&apos;." verbose="Finding the same code in an &apos;if&apos; and related &apos;else&apos; branch is suspicious and might indicate a cut and paste or logic error. Please examine this code carefully to determine if it is correct." cwe="398" inconclusive="true"/>
        <error id="duplicateExpression" severity="style" msg="Same expression on both sides of &apos;&amp;&amp;&apos;." verbose="Finding the same expression on both sides of an operator is suspicious and might indicate a cut and paste or logic error. Please examine this code carefully to determine if it is correct." cwe="398"/>
        <error id="duplicateExpressionTernary" severity="style" msg="Same expression in both branches of ternary operator." verbose="Finding the same expression in both branches of ternary operator is suspicious as the same code is executed regardless of the condition." cwe="398"/>
        <error id="duplicateBreak" severity="style" msg="Consecutive return, break, continue, goto or throw statements are unnecessary." verbose="Consecutive return, break, continue, goto or throw statements are unnecessary. The second statement can never be executed, and so should be removed." cwe="561"/>
        <error id="unreachableCode" severity="style" msg="Statements following return, break, continue, goto or throw will never be executed." verbose="Statements following return, break, continue, goto or throw will never be executed." cwe="561"/>
        <error id="unsignedLessThanZero" severity="style" msg="Checking if unsigned variable &apos;varname&apos; is less than zero." verbose="The unsigned variable &apos;varname&apos; will never be negative so it is either pointless or an error to check if it is." cwe="570"/>
        <error id="unsignedPositive" severity="style" msg="Unsigned variable &apos;varname&apos; can&apos;t be negative so it is unnecessary to test it." verbose="Unsigned variable &apos;varname&apos; can&apos;t be negative so it is unnecessary to test it." cwe="570"/>
        <error id="pointerLessThanZero" severity="style" msg="A pointer can not be negative so it is either pointless or an error to check if it is." verbose="A pointer can not be negative so it is either pointless or an error to check if it is." cwe="570"/>
        <error id="pointerPositive" severity="style" msg="A pointer can not be negative so it is either pointless or an error to check if it is not." verbose="A pointer can not be negative so it is either pointless or an error to check if it is not." cwe="570"/>
        <error id="suspiciousSemicolon" severity="warning" msg="Suspicious use of ; at the end of &apos;&apos; statement." verbose="Suspicious use of ; at the end of &apos;&apos; statement." cwe="398" inconclusive="true"/>
        <error id="incompleteArrayFill" severity="warning" msg="Array &apos;buffer&apos; is filled incompletely. Did you forget to multiply the size given to &apos;memset()&apos; with &apos;sizeof(*buffer)&apos;?" verbose="The array &apos;buffer&apos; is filled incompletely. The function &apos;memset()&apos; needs the size given in bytes, but an element of the given array is larger than one byte. Did you forget to multiply the size with &apos;sizeof(*buffer)&apos;?" cwe="131" inconclusive="true"/>
        <error id="varFuncNullUB" severity="portability" msg="Passing NULL after the last typed argument to a variadic function leads to undefined behaviour." verbose="Passing NULL after the last typed argument to a variadic function leads to undefined behaviour.\012The C99 standard, in section 7.15.1.1, states that if the type used by va_arg() is not compatible with the type of the actual next argument (as promoted according to the default argument promotions), the behavior is undefined.\012The value of the NULL macro is an implementation-defined null pointer constant (7.17), which can be any integer constant expression with the value 0, or such an expression casted to (void*) (6.3.2.3). This includes values like 0, 0L, or even 0LL.\012In practice on common architectures, this will cause real crashes if sizeof(int) != sizeof(void*), and NULL is defined to 0 or any other null pointer constant that promotes to int.\012To reproduce you might be able to use this little code example on 64bit platforms. If the output includes &quot;ERROR&quot;, the sentinel had only 4 out of 8 bytes initialized to zero and was not detected as the final argument to stop argument processing via va_arg(). Changing the 0 to (void*)0 or 0L will make the &quot;ERROR&quot; output go away.\012#include &lt;stdarg.h&gt;\012#include &lt;stdio.h&gt;\012\012void f(char *s, ...) {\012    va_list ap;\012    va_start(ap,s);\012    for (;;) {\012        char *p = va_arg(ap,char*);\012        printf(&quot;%018p, %s\n&quot;, p, (long)p &amp; 255 ? p : &quot;&quot;);\012        if(!p) break;\012    }\012    va_end(ap);\012}\012\012void g() {\012    char *s2 = &quot;x&quot;;\012    char *s3 = &quot;ERROR&quot;;\012\012    // changing 0 to 0L for the 7th argument (which is intended to act as sentinel) makes the error go away on x86_64\012    f(&quot;first&quot;, s2, s2, s2, s2, s2, 0, s3, (char*)0);\012}\012\012void h() {\012    int i;\012    volatile unsigned char a[1000];\012    for (i = 0; i&lt;sizeof(a); i++)\012        a[i] = -1;\012}\012\012int main() {\012    h();\012    g();\012    return 0;\012}" cwe="475"/>
        <error id="nanInArithmeticExpression" severity="style" msg="Using NaN/Inf in a computation." verbose="Using NaN/Inf in a computation. Although nothing bad really happens, it is suspicious." cwe="369"/>
        <error id="commaSeparatedReturn" severity="style" msg="Comma is used in return statement. The comma can easily be misread as a &apos;;&apos;." verbose="Comma is used in return statement. When comma is used in a return statement it can easily be misread as a semicolon. For example in the code below the value of &apos;b&apos; is returned if the condition is true, but it is easy to think that &apos;a+1&apos; is returned:\012    if (x)\012        return a + 1,\012    b++;\012However it can be useful to use comma in macros. Cppcheck does not warn when such a macro is then used in a return statement, it is less likely such code is misunderstood." cwe="398"/>
        <error id="redundantPointerOp" severity="style" msg="Redundant pointer operation on &apos;varname&apos; - it&apos;s already a pointer." verbose="Redundant pointer operation on &apos;varname&apos; - it&apos;s already a pointer." cwe="398"/>
        <error id="unusedLabelSwitch" severity="warning" msg="Label &apos;&apos; is not used. Should this be a &apos;case&apos; of the enclosing switch()?" verbose="Label &apos;&apos; is not used. Should this be a &apos;case&apos; of the enclosing switch()?" cwe="398"/>
        <error id="unusedLabel" severity="style" msg="Label &apos;&apos; is not used." verbose="Label &apos;&apos; is not used." cwe="398"/>
        <error id="unknownEvaluationOrder" severity="error" msg="Expression &apos;x = x++;&apos; depends on order of evaluation of side effects" verbose="Expression &apos;x = x++;&apos; depends on order of evaluation of side effects" cwe="768"/>
        <error id="accessMoved" severity="warning" msg="Access of moved variable &apos;v&apos;." verbose="Access of moved variable &apos;v&apos;." cwe="672"/>
        <error id="accessForwarded" severity="warning" msg="Access of forwarded variable &apos;v&apos;." verbose="Access of forwarded variable &apos;v&apos;." cwe="672"/>
        <error id="funcArgNamesDifferent" severity="style" msg="Function &apos;function&apos; argument 2 names different: declaration &apos;A&apos; definition &apos;B&apos;." verbose="Function &apos;function&apos; argument 2 names different: declaration &apos;A&apos; definition &apos;B&apos;." cwe="628" inconclusive="true"/>
        <error id="funcArgOrderDifferent" severity="warning" msg="Function &apos;function&apos; argument order different: declaration &apos;&apos; definition &apos;&apos;" verbose="Function &apos;function&apos; argument order different: declaration &apos;&apos; definition &apos;&apos;" cwe="683"/>
        <error id="invalidIterator1" severity="error" msg="Invalid iterator: iterator" verbose="Invalid iterator: iterator" cwe="664"/>
        <error id="iterators" severity="error" msg="Same iterator is used with different containers &apos;container1&apos; and &apos;container2&apos;." verbose="Same iterator is used with different containers &apos;container1&apos; and &apos;container2&apos;." cwe="664"/>
        <error id="mismatchingContainers" severity="error" msg="Iterators of different containers are used together." verbose="Iterators of different containers are used together." cwe="664"/>
        <error id="eraseDereference" severity="error" msg="Invalid iterator &apos;iter&apos; used." verbose="The iterator &apos;iter&apos; is invalid before being assigned. Dereferencing or comparing it with another iterator is invalid operation." cwe="664"/>
        <error id="stlOutOfBounds" severity="error" msg="When i==foo.size(), foo[i] is out of bounds." verbose="When i==foo.size(), foo[i] is out of bounds." cwe="788"/>
        <error id="invalidIterator2" severity="error" msg="After push_back|push_front|insert(), the iterator &apos;iterator&apos; may be invalid." verbose="After push_back|push_front|insert(), the iterator &apos;iterator&apos; may be invalid." cwe="664"/>
        <error id="invalidPointer" severity="error" msg="Invalid pointer &apos;pointer&apos; after push_back()." verbose="Invalid pointer &apos;pointer&apos; after push_back()." cwe="664"/>
        <error id="stlBoundaries" severity="error" msg="Dangerous comparison using operator&lt; on iterator." verbose="Iterator compared with operator&lt;. This is dangerous since the order of items in the container is not guaranteed. One should use operator!= instead to compare iterators." cwe="664"/>
        <error id="stlIfFind" severity="warning" msg="Suspicious condition. The result of find() is an iterator, but it is not properly checked." verbose="Suspicious condition. The result of find() is an iterator, but it is not properly checked." cwe="398"/>
        <error id="stlIfStrFind" severity="performance" msg="Inefficient usage of string::find() in condition; string::compare() would be faster." verbose="Either inefficient or wrong usage of string::find(). string::compare() will be faster if string::find&apos;s result is compared with 0, because it will not scan the whole string. If your intention is to check that there are no findings in the string, you should compare with std::string::npos." cwe="597"/>
        <error id="stlcstr" severity="error" msg="Dangerous usage of c_str(). The value returned by c_str() is invalid after this call." verbose="Dangerous usage of c_str(). The c_str() return value is only valid until its string is deleted." cwe="664"/>
        <error id="stlcstrReturn" severity="performance" msg="Returning the result of c_str() in a function that returns std::string is slow and redundant." verbose="The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly returning the string." cwe="704"/>
        <error id="stlcstrParam" severity="performance" msg="Passing the result of c_str() to a function that takes std::string as argument no. 0 is slow and redundant." verbose="The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly passing the string." cwe="704"/>
        <error id="stlSize" severity="performance" msg="Possible inefficient checking for &apos;list&apos; emptiness." verbose="Checking for &apos;list&apos; emptiness might be inefficient. Using list.empty() instead of list.size() can be faster. list.size() can take linear time but list.empty() is guaranteed to take constant time." cwe="398"/>
        <error id="StlMissingComparison" severity="warning" msg="Missing bounds check for extra iterator increment in loop." verbose="The iterator incrementing is suspicious - it is incremented at line  and then at line . The loop might unintentionally skip an element in the container. There is no comparison between these increments to prevent that the iterator is incremented beyond the end." cwe="834"/>
        <error id="redundantIfRemove" severity="style" msg="Redundant checking of STL container element existence before removing it." verbose="Redundant checking of STL container element existence before removing it. It is safe to call the remove method on a non-existing element." cwe="398"/>
        <error id="useAutoPointerCopy" severity="style" msg="Copying &apos;auto_ptr&apos; pointer to another does not create two equal objects since one has lost its ownership of the pointer." verbose="&apos;std::auto_ptr&apos; has semantics of strict ownership, meaning that the &apos;auto_ptr&apos; instance is the sole entity responsible for the object&apos;s lifetime. If an &apos;auto_ptr&apos; is copied, the source looses the reference." cwe="398"/>
        <error id="useAutoPointerContainer" severity="error" msg="You can randomly lose access to pointers if you store &apos;auto_ptr&apos; pointers in an STL container." verbose="An element of container must be able to be copied but &apos;auto_ptr&apos; does not fulfill this requirement. You should consider to use &apos;shared_ptr&apos; or &apos;unique_ptr&apos;. It is suitable for use in containers, because they no longer copy their values, they move them." cwe="664"/>
        <error id="useAutoPointerArray" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new[]&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes arrays, which are allocated by operator &apos;new[]&apos; and must be deallocated by operator &apos;delete[]&apos;." cwe="664"/>
        <error id="useAutoPointerMalloc" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes use C library allocation functions (for example &apos;malloc&apos;), which must be deallocated by the appropriate C library function." cwe="762"/>
        <error id="uselessCallsCompare" severity="warning" msg="It is inefficient to call &apos;str.find(str)&apos; as it always returns 0." verbose="&apos;std::string::find()&apos; returns zero when given itself as parameter (str.find(str)). As it is currently the code is inefficient. It is possible either the string searched (&apos;str&apos;) or searched for (&apos;str&apos;) is wrong." cwe="628"/>
        <error id="uselessCallsSwap" severity="performance" msg="It is inefficient to swap a object with itself by calling &apos;str.swap(str)&apos;" verbose="The &apos;swap()&apos; function has no logical effect when given itself as parameter (str.swap(str)). As it is currently the code is inefficient. Is the object or the parameter wrong here?" cwe="628"/>
        <error id="uselessCallsSubstr" severity="performance" msg="Ineffective call of function &apos;substr&apos; because it returns a copy of the object. Use operator= instead." verbose="Ineffective call of function &apos;substr&apos; because it returns a copy of the object. Use operator= instead." cwe="398"/>
        <error id="uselessCallsEmpty" severity="warning" msg="Ineffective call of function &apos;empty()&apos;. Did you intend to call &apos;clear()&apos; instead?" verbose="Ineffective call of function &apos;empty()&apos;. Did you intend to call &apos;clear()&apos; instead?" cwe="398"/>
        <error id="uselessCallsRemove" severity="warning" msg="Return value of std::remove() ignored. Elements remain in container." verbose="The return value of std::remove() is ignored. This function returns an iterator to the end of the range containing those elements that should be kept. Elements past new end remain valid but with unspecified values. Use the erase method of the container to delete them." cwe="762"/>
        <error id="derefInvalidIterator" severity="warning" msg="Possible dereference of an invalid iterator: i" verbose="Make sure to check that the iterator is valid before dereferencing it - not after." cwe="825"/>
        <error id="reademptycontainer" severity="style" msg="Reading from empty STL container &apos;var&apos;" verbose="Reading from empty STL container &apos;var&apos;" cwe="398" inconclusive="true"/>
        <error id="sizeofwithsilentarraypointer" severity="warning" msg="Using &apos;sizeof&apos; on array given as function argument returns size of a pointer." verbose="Using &apos;sizeof&apos; for array given as function argument returns the size of a pointer. It does not return the size of the whole array in bytes as might be expected. For example, this code:\012     int f(char a[100]) {\012         return sizeof(a);\012     }\012returns 4 (in 32-bit systems) or 8 (in 64-bit systems) instead of 100 (the size of the array in bytes)." cwe="467"/>
        <error id="pointerSize" severity="warning" msg="Size of pointer &apos;varname&apos; used instead of size of its data." verbose="Size of pointer &apos;varname&apos; used instead of size of its data. This is likely to lead to a buffer overflow. You probably intend to write &apos;sizeof(*varname)&apos;." cwe="467"/>
        <error id="sizeofDivisionMemfunc" severity="warning" msg="Division by result of sizeof(). memset() expects a size in bytes, did you intend to multiply instead?" verbose="Division by result of sizeof(). memset() expects a size in bytes, did you intend to multiply instead?" cwe="682"/>
        <error id="sizeofwithnumericparameter" severity="warning" msg="Suspicious usage of &apos;sizeof&apos; with a numeric constant as parameter." verbose="It is unusual to use a constant value with sizeof. For example, &apos;sizeof(10)&apos; returns 4 (in 32-bit systems) or 8 (in 64-bit systems) instead of 10. &apos;sizeof(&apos;A&apos;)&apos; and &apos;sizeof(char)&apos; can return different results." cwe="682"/>
        <error id="sizeofsizeof" severity="warning" msg="Calling &apos;sizeof&apos; on &apos;sizeof&apos;." verbose="Calling sizeof for &apos;sizeof looks like a suspicious code and most likely there should be just one &apos;sizeof&apos;. The current code is equivalent to &apos;sizeof(size_t)&apos;" cwe="682"/>
        <error id="sizeofCalculation" severity="warning" msg="Found calculation inside sizeof()." verbose="Found calculation inside sizeof()." cwe="682"/>
        <error id="sizeofFunctionCall" severity="warning" msg="Found function call inside sizeof()." verbose="Found function call inside sizeof()." cwe="682"/>
        <error id="multiplySizeof" severity="warning" msg="Multiplying sizeof() with sizeof() indicates a logic error." verbose="Multiplying sizeof() with sizeof() indicates a logic error." cwe="682" inconclusive="true"/>
        <error id="divideSizeof" severity="warning" msg="Division of result of sizeof() on pointer type." verbose="Division of result of sizeof() on pointer type. sizeof() returns the size of the pointer, not the size of the memory area it points to." cwe="682" inconclusive="true"/>
        <error id="sizeofVoid" severity="portability" msg="Behaviour of &apos;sizeof(void)&apos; is not covered by the ISO C standard." verbose="Behaviour of &apos;sizeof(void)&apos; is not covered by the ISO C standard. A value for &apos;sizeof(void)&apos; is defined only as part of a GNU C extension, which defines &apos;sizeof(void)&apos; to be 1." cwe="682"/>
        <error id="sizeofDereferencedVoidPointer" severity="portability" msg="&apos;*varname&apos; is of type &apos;void&apos;, the behaviour of &apos;sizeof(void)&apos; is not covered by the ISO C standard." verbose="&apos;*varname&apos; is of type &apos;void&apos;, the behaviour of &apos;sizeof(void)&apos; is not covered by the ISO C standard. A value for &apos;sizeof(void)&apos; is defined only as part of a GNU C extension, which defines &apos;sizeof(void)&apos; to be 1." cwe="682"/>
        <error id="arithOperationsOnVoidPointer" severity="portability" msg="&apos;varname&apos; is of type &apos;vartype&apos;. When using void pointers in calculations, the behaviour is undefined." verbose="&apos;varname&apos; is of type &apos;vartype&apos;. When using void pointers in calculations, the behaviour is undefined. Arithmetic operations on &apos;void *&apos; is a GNU C extension, which defines the &apos;sizeof(void)&apos; to be 1." cwe="467"/>
        <error id="stringLiteralWrite" severity="error" msg="Modifying string literal directly or indirectly is undefined behaviour." verbose="Modifying string literal directly or indirectly is undefined behaviour." cwe="758"/>
        <error id="sprintfOverlappingData" severity="error" msg="Undefined behavior: Variable &apos;varname&apos; is used as parameter and destination in s[n]printf()." verbose="The variable &apos;varname&apos; is used both as a parameter and as destination in s[n]printf(). The origin and destination buffers overlap. Quote from glibc (C-library) documentation (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Formatted-Output-Functions): &quot;If copying takes place between objects that overlap as a result of a call to sprintf() or snprintf(), the results are undefined.&quot;" cwe="628"/>
        <error id="strPlusChar" severity="error" msg="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." verbose="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." cwe="665"/>
        <error id="incorrectStringCompare" severity="warning" msg="String literal &quot;Hello World&quot; doesn&apos;t match length argument for substr()." verbose="String literal &quot;Hello World&quot; doesn&apos;t match length argument for substr()." cwe="570"/>
        <error id="literalWithCharPtrCompare" severity="warning" msg="String literal compared with variable &apos;foo&apos;. Did you intend to use strcmp() instead?" verbose="String literal compared with variable &apos;foo&apos;. Did you intend to use strcmp() instead?" cwe="595"/>
        <error id="charLiteralWithCharPtrCompare" severity="warning" msg="Char literal compared with pointer &apos;foo&apos;. Did you intend to dereference it?" verbose="Char literal compared with pointer &apos;foo&apos;. Did you intend to dereference it?" cwe="595"/>
        <error id="incorrectStringBooleanError" severity="warning" msg="Conversion of string literal &quot;Hello World&quot; to bool always evaluates to true." verbose="Conversion of string literal &quot;Hello World&quot; to bool always evaluates to true." cwe="571"/>
        <error id="staticStringCompare" severity="warning" msg="Unnecessary comparison of static strings." verbose="The compared strings, &apos;str1&apos; and &apos;str2&apos;, are always unequal. Therefore the comparison is unnecessary and looks suspicious." cwe="570"/>
        <error id="stringCompare" severity="warning" msg="Comparison of identical string variables." verbose="The compared strings, &apos;varname1&apos; and &apos;varname2&apos;, are identical. This could be a logic bug." cwe="571"/>
        <error id="overlappingStrcmp" severity="warning" msg="The expression &apos;strcmp(x,&quot;def&quot;) != 0&apos; is suspicious. It overlaps &apos;strcmp(x,&quot;abc&quot;) == 0&apos;." verbose="The expression &apos;strcmp(x,&quot;def&quot;) != 0&apos; is suspicious. It overlaps &apos;strcmp(x,&quot;abc&quot;) == 0&apos;."/>
        <error id="shiftTooManyBits" severity="error" msg="Shifting 32-bit value by 40 bits is undefined behaviour" verbose="Shifting 32-bit value by 40 bits is undefined behaviour" cwe="758"/>
        <error id="shiftTooManyBitsSigned" severity="error" msg="Shifting signed 32-bit value by 31 bits is undefined behaviour" verbose="Shifting signed 32-bit value by 31 bits is undefined behaviour" cwe="758"/>
        <error id="integerOverflow" severity="error" msg="Signed integer overflow for expression &apos;&apos;." verbose="Signed integer overflow for expression &apos;&apos;." cwe="190"/>
        <error id="signConversion" severity="warning" msg="Suspicious code: sign conversion of var in calculation, even though var can have a negative value" verbose="Suspicious code: sign conversion of var in calculation, even though var can have a negative value" cwe="195"/>
        <error id="truncLongCastAssignment" severity="style" msg="int result is assigned to long variable. If the variable is long to avoid loss of information, then you have loss of information." verbose="int result is assigned to long variable. If the variable is long to avoid loss of information, then there is loss of information. To avoid loss of information you must cast a calculation operand to long, for example &apos;l = a * b;&apos; =&gt; &apos;l = (long)a * b;&apos;." cwe="197"/>
        <error id="truncLongCastReturn" severity="style" msg="int result is returned as long value. If the return value is long to avoid loss of information, then you have loss of information." verbose="int result is returned as long value. If the return value is long to avoid loss of information, then there is loss of information. To avoid loss of information you must cast a calculation operand to long, for example &apos;return a*b;&apos; =&gt; &apos;return (long)a*b&apos;." cwe="197"/>
        <error id="floatConversionOverflow" severity="error" msg="Undefined behaviour: float (1e+100) to integer conversion overflow." verbose="Undefined behaviour: float (1e+100) to integer conversion overflow." cwe="190"/>
        <error id="uninitstring" severity="error" msg="Dangerous usage of &apos;varname&apos; (strncpy doesn&apos;t always null-terminate it)." verbose="Dangerous usage of &apos;varname&apos; (strncpy doesn&apos;t always null-terminate it)." cwe="676"/>
        <error id="uninitdata" severity="error" msg="Memory is allocated but not initialized: varname" verbose="Memory is allocated but not initialized: varname" cwe="908"/>
        <error id="uninitvar" severity="error" msg="Uninitialized variable: varname" verbose="Uninitialized variable: varname" cwe="908"/>
        <error id="uninitStructMember" severity="error" msg="Uninitialized struct member: a.b" verbose="Uninitialized struct member: a.b" cwe="908"/>
        <error id="deadpointer" severity="error" msg="Dead pointer usage. Pointer &apos;pointer&apos; is dead if it has been assigned &apos;&amp;x&apos; at line 0." verbose="Dead pointer usage. Pointer &apos;pointer&apos; is dead if it has been assigned &apos;&amp;x&apos; at line 0." cwe="825"/>
        <error id="unusedFunction" severity="style" msg="The function &apos;funcName&apos; is never used." verbose="The function &apos;funcName&apos; is never used." cwe="561"/>
        <error id="unusedVariable" severity="style" msg="Unused variable: varname" verbose="Unused variable: varname" cwe="563"/>
        <error id="unusedAllocatedMemory" severity="style" msg="Variable &apos;varname&apos; is allocated memory that is never used." verbose="Variable &apos;varname&apos; is allocated memory that is never used." cwe="563"/>
        <error id="unreadVariable" severity="style" msg="Variable &apos;varname&apos; is assigned a value that is never used." verbose="Variable &apos;varname&apos; is assigned a value that is never used." cwe="563"/>
        <error id="unassignedVariable" severity="style" msg="Variable &apos;varname&apos; is not assigned a value." verbose="Variable &apos;varname&apos; is not assigned a value." cwe="665"/>
        <error id="unusedStructMember" severity="style" msg="struct member &apos;structname::variable&apos; is never used." verbose="struct member &apos;structname::variable&apos; is never used." cwe="563"/>
        <error id="postfixOperator" severity="performance" msg="Prefer prefix ++/-- operators for non-primitive types." verbose="Prefix ++/-- operators should be preferred for non-primitive types. Pre-increment/decrement can be more efficient than post-increment/decrement. Post-increment/decrement usually involves keeping a copy of the previous value around and adds a little extra code." cwe="398"/>
        <error id="va_start_wrongParameter" severity="warning" msg="&apos;arg1&apos; given to va_start() is not last named argument of the function. Did you intend to pass &apos;arg2&apos;?" verbose="&apos;arg1&apos; given to va_start() is not last named argument of the function. Did you intend to pass &apos;arg2&apos;?" cwe="688"/>
        <error id="va_start_referencePassed" severity="error" msg="Using reference &apos;arg1&apos; as parameter for va_start() results in undefined behaviour." verbose="Using reference &apos;arg1&apos; as parameter for va_start() results in undefined behaviour." cwe="758"/>
        <error id="va_end_missing" severity="error" msg="va_list &apos;vl&apos; was opened but not closed by va_end()." verbose="va_list &apos;vl&apos; was opened but not closed by va_end()." cwe="664"/>
        <error id="va_list_usedBeforeStarted" severity="error" msg="va_list &apos;vl&apos; used before va_start() was called." verbose="va_list &apos;vl&apos; used before va_start() was called." cwe="664"/>
        <error id="va_start_subsequentCalls" severity="error" msg="va_start() or va_copy() called subsequently on &apos;vl&apos; without va_end() in between." verbose="va_start() or va_copy() called subsequently on &apos;vl&apos; without va_end() in between." cwe="664"/>
        <error id="missingInclude" severity="information" msg="Include file: &quot;&quot; not found." verbose="Include file: &quot;&quot; not found."/>
        <error id="missingIncludeSystem" severity="information" msg="Include file: &lt;&gt; not found. Please note: Cppcheck does not need standard library headers to get proper results." verbose="Include file: &lt;&gt; not found. Please note: Cppcheck does not need standard library headers to get proper results."/>
        <error id="ConfigurationNotChecked" severity="information" msg="Skipping configuration &apos;X&apos; since the value of &apos;X&apos; is unknown. Use -D if you want to check it. You can use -U to skip it explicitly." verbose="Skipping configuration &apos;X&apos; since the value of &apos;X&apos; is unknown. Use -D if you want to check it. You can use -U to skip it explicitly.">
        <error id="preprocessorErrorDirective" severity="error" msg="#error message" verbose="#error message"/>
    </errors>
	</results>

## 3. Relevant Documentation
- Home Page: [http://cppcheck.sourceforge.net/](http://cppcheck.sourceforge.net/)
- Wiki: [https://sourceforge.net/p/cppcheck/wiki/Home/](https://sourceforge.net/p/cppcheck/wiki/Home/)
- Manual: [http://cppcheck.sourceforge.net/manual.pdf](http://cppcheck.sourceforge.net/manual.pdf)
- Writing Rules: [http://sourceforge.net/projects/cppcheck/files/Articles/](http://sourceforge.net/projects/cppcheck/files/Articles/)
- CWE Type: [https://cwe.mitre.org/data/definitions/659.html](https://cwe.mitre.org/data/definitions/659.html)