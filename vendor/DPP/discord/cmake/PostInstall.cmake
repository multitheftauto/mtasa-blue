if (RUN_LDCONFIG)
	if(LDCONFIG_EXECUTABLE)
		message(STATUS "Running ldconfig")
		
		execute_process(COMMAND ${LDCONFIG_EXECUTABLE} RESULT_VARIABLE ldconfig_result)
		
		if (NOT ldconfig_result EQUAL 0)
			message(WARNING "ldconfig failed")
		endif()
	endif()
endif()