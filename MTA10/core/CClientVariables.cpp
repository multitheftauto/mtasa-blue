/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CClientVariables.cpp
*  PURPOSE:		Managed storage of client variables (cvars)
*  DEVELOPERS:	Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

template<> CClientVariables * CSingleton< CClientVariables >::m_pSingleton = NULL;

CClientVariables::CClientVariables ( )
{
    WriteDebugEvent ( "CClientVariables::CClientVariables" );
}

CClientVariables::~CClientVariables ( )
{
    WriteDebugEvent ( "CClientVariables::~CClientVariables" );
}

bool CClientVariables::CreateVariable ( string VariableName, 
                                        string VariableValue )
{
    CVARENTRY Variable;

    // Copy in the name.
    strncpy ( Variable.szVariableName, 
              VariableName.c_str ( ), 
              GET_LENGTH_BOUND ( MAX_CVAR_NAME_LENGTH, VariableName.size ( ) ) );
    
    // Copy in the value.
    strncpy ( Variable.szVariableValue,
              VariableValue.c_str ( ),
              GET_LENGTH_BOUND ( MAX_CVAR_VALUE_LENGTH, VariableValue.size ( ) ) );

    // Push it onto the list.
    m_VariableList.insertAtFront ( Variable );

    return false;
}

bool CClientVariables::DeleteVariable ( string VariableName )
{
	CLinkedList< CVARENTRY >::CIterator iterator;

    // Find the entry we're looking for.
	for ( iterator = m_VariableList.getBegin (); 
          iterator != m_VariableList.getEnd (); 
          iterator++ ) 
    {
          // Check to see if this is the variable.
          if ( !stricmp( VariableName.c_str ( ), iterator->szVariableName ) ) 
          { 
              // It is, so remove it.
              if ( !m_VariableList.isEmpty () ) m_VariableList.remove ( iterator ); 
              return true; 
          }    
	}

	return false;
}

bool CClientVariables::DoesVariableExist ( string VariableName )
{
	CLinkedList< CVARENTRY >::CIterator iterator;

    // Find the entry we're looking for.
	for ( iterator = m_VariableList.getBegin (); 
          iterator != m_VariableList.getEnd (); 
          iterator++ ) 
    {
          // Check to see if this is the variable.
          if ( !stricmp( VariableName.c_str ( ), iterator->szVariableName ) ) 
          { 
              // It is, so return true.
              return true; 
          }    
	}

    return false;
}