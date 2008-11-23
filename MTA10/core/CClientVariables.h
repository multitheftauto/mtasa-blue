/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CClientVariables.h
*  PURPOSE:		Header file for client variable class
*  DEVELOPERS:	Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTVARIABLES_H
#define __CCLIENTVARIABLES_H

#include <string>
#include "CLinkedList.h"
#include "CSingleton.h"

using namespace std;

#define MAX_CVAR_VALUE_LENGTH   256
#define MAX_CVAR_NAME_LENGTH    32

typedef struct tagCVARENTRY
{
    char    szVariableName[MAX_CVAR_NAME_LENGTH];
    char    szVariableValue[MAX_CVAR_VALUE_LENGTH];
} CVARENTRY;

class CClientVariables : public CSingleton < CClientVariables >
{
     public:
            CClientVariables        ( );
           ~CClientVariables        ( );

     bool   CreateVariable          ( string VariableName, string VariableValue = "" );
     bool   DeleteVariable          ( string VariableName );
     bool   DoesVariableExist       ( string VariableName );  
     
     private:

     CLinkedList < CVARENTRY >  m_VariableList;
};

#endif