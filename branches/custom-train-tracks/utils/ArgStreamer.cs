/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/ArgStreamer.cs
*  PURPOSE:     Lua to argStream conveter + parser
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;

class ArgStreamer
{
    public ArgStreamer()
    {
    }

    /// <summary>
    /// Parse the given file
    /// </summary>
    /// <param name="file">The file path to parse</param>
    /// <returns>A List containing the new code of each function</returns>
    public ArrayList ParseFile(string file)
    {
        string content = File.ReadAllText(file);
        string className = file.Substring(0, file.IndexOf(".cpp")); // ToDo: Replace by regex (read content)
        ArrayList newCodes = new ArrayList();

        Dictionary<string, string> functions = GetFunctions(content);
        foreach (var pair in functions)
        {
            string funcName = pair.Key;
            string funcContent = pair.Value;
            ArrayList parameters = GetParameters(funcContent);

            Console.WriteLine("Parsing " + funcName);

            string newCode = "";
            newCode += "// " + GetSyntaxString(funcName) + "\n";
            ArrayList names;

            foreach (string declaration in GetParameterDeclarations(funcContent, out names))
            {
                newCode += declaration + " ";
            }
            newCode += "\n\n";

            newCode += "CScriptArgReader argStream ( luaVM );\n";
            int i = 0;
            foreach (string luaType in parameters)
            {
                string argStreamType = GetArgStreamTypeFromLuaType(luaType);
                if ( names.Count > i )
                    newCode += "argStream.Read" + argStreamType + " ( " + names[i] + " );\n";
                i++;
            }
            newCode += "\n";
            newCode += "if ( !argStream.HasErrors () )\n";
            newCode += "{\n";
            newCode += removeTabs(GetFunctionBody(funcContent), 2);
            newCode += "}\n";
            newCode += "else\n";
            newCode += "    m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );\n";

            newCodes.Add(addTabs(newCode, 1));
        }

        return newCodes;
    }

    /// <summary>
    /// Get all functions (name + code)
    /// </summary>
    /// <param name="content">Content of the file</param>
    /// <returns>A dictionary containing all functions (Key = function name, Value = function code)</returns>
    private Dictionary<string, string> GetFunctions(string content)
    {
        Dictionary<string, string> functions = new Dictionary<string, string>();
        
        MatchCollection matches = Regex.Matches(content, @"int (\w*)::(.*?) \( lua_State\* luaVM \)\r\n{(.*?)\r\n}", RegexOptions.Singleline);
        foreach (Match match in matches)
        {
            string funcName = match.Groups[2].Value;
            string funcContent = match.Groups[3].Value;
            functions.Add(funcName, funcContent);
        }

        return functions;
    }

    /// <summary>
    /// Get the parameters of the given function
    /// </summary>
    /// <param name="funcContent">The function's code</param>
    /// <returns>A list containing all parameters</returns>
    private ArrayList GetParameters(string funcContent)
    {
        ArrayList parameters = new ArrayList();

        MatchCollection matches = Regex.Matches(funcContent, @"\( lua_type \( luaVM, (\d) \) == (.*?) ", RegexOptions.Singleline);
        foreach (Match match in matches)
        {
            string paramType = match.Groups[2].Value;
            parameters.Add(paramType);
        }

        return parameters;
    }

    /// <summary>
    /// Get parameter translation (used by transforming)
    /// </summary>
    /// <param name="funcContent">The function's code</param>
    /// <param name="names">List containing all names (out)</param>
    /// <returns>A list containing the function declarations</returns>
    private ArrayList GetParameterDeclarations(string funcContent, out ArrayList names)
    {
        ArrayList paramDeclarations = new ArrayList();
        names = new ArrayList();

        Match match = Regex.Match(funcContent, @"if \( (.*?) \)\r\n    {\r\n(.*?)\r\n        if", RegexOptions.Singleline);
        if (match.Success)
        {
            string parameterReading = match.Groups[2].Value;
            
            // Get variable type + name
            MatchCollection matches = Regex.Matches(funcContent, @"        (.*?) (.*?) = lua_to(\w*)", RegexOptions.Multiline);
            foreach (Match m in matches)
            {
                string type = m.Groups[1].Value;
                string name = m.Groups[2].Value;
                if (name.IndexOf("char*") != -1)
                {
                    type = "const char*";
                    name = name.Substring("char*".Length + 1);
                }

                string paramDeclaration = type + " " + name + ";";
                paramDeclarations.Add(paramDeclaration);
                names.Add(name);
            }
        }

        return paramDeclarations;
    }

    /// <summary>
    /// Get the main code of the function (except lua parameter reading)
    /// </summary>
    /// <param name="funcContent"></param>
    /// <returns></returns>
    private string GetFunctionBody(string funcContent)
    {
        Match match = Regex.Match(funcContent, @"(.*?)        {\r\n(.*?)\r\n        }", RegexOptions.Singleline);
        if (match.Success)
            return match.Groups[2].Value;

        return "!!!Missing function body!!!";
    }

    private string addTabs(string text, int tabs)
    {
        string buffer = "";
        using (StringReader reader = new StringReader(text))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                for (int i = 1; i <= tabs; i++)
                {
                    buffer += "    ";
                }
                buffer += line + "\n";
            }
        }
        return buffer;
    }

    /// <summary>
    /// Replace tabs (= x spaces) in each line
    /// </summary>
    /// <param name="text"></param>
    /// <param name="tabs"></param>
    /// <returns></returns>
    private string removeTabs(string text, int tabs)
    {
        string buffer = "";
        using (StringReader reader = new StringReader(text))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                if (line.Length >= (tabs*4))
                    buffer += line.Substring(tabs * 4) + "\n";
            }
        }
        return buffer;
    }

    /// <summary>
    /// Get the type for CScriptArgReader from a lua type
    /// </summary>
    /// <param name="luaType">The lua type</param>
    /// <returns>Type for CScriptArgReader</returns>
    private string GetArgStreamTypeFromLuaType(string luaType)
    {
        switch (luaType)
        {
            case "LUA_TBOOLEAN":
                return "Bool";
            case "LUA_TNUMBER":
                return "Number";
            case "LUA_TSTRING":
                return "String";
            case "LUA_TLIGHTUSERDATA":
                return "UserData";
        }
        return null;
    }

    /// <summary>
    /// Get the syntax string from the wiki
    /// </summary>
    /// <param name="page">The name of the function you wish to look up</param>
    /// <returns>The function syntax string</returns>
    public string GetSyntaxString(string page)
    {
        WebRequest request = WebRequest.Create("http://wiki.multitheftauto.com/wiki/" + page);
        HttpWebResponse response;
        try
        {
            response = (HttpWebResponse)request.GetResponse();
        }
        catch
        {
            return "Error";
        }
        Stream dataStream = response.GetResponseStream();
        StreamReader reader = new StreamReader(dataStream);
        string strHTML = reader.ReadToEnd();

        int iStartIndex = strHTML.IndexOf(@"<!-- PLAIN TEXT CODE FOR BOTS |") + (@"<!-- PLAIN TEXT CODE FOR BOTS |").Length;
        int iEndIndex = strHTML.IndexOf(@"|-->");

        if (iEndIndex <= iStartIndex)
            return "Error";

        string strSyntax = strHTML.Substring(iStartIndex, iEndIndex - iStartIndex);

        reader.Close();
        dataStream.Close();
        response.Close();

        return strSyntax;
    }
}
