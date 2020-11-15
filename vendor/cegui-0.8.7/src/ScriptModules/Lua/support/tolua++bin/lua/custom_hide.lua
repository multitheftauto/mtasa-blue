-- extract code with tolua_hide, and also
function extract_code(fn,s)
	local code = '\n$#include "'..fn..'"\n'
	s= "\n" .. s .. "\n" -- add blank lines as sentinels
	local _,e,c,t = strfind(s, "\n([^\n]-)[Tt][Oo][Ll][Uu][Aa]_([^%s]*)[^\n]*\n")
	while e do
		t = strlower(t)
		if t == "begin" then
			_,e,c = strfind(s,"(.-)\n[^\n]*[Tt][Oo][Ll][Uu][Aa]_[Ee][Nn][Dd][^\n]*\n",e)
			if not e then
			 tolua_error("Unbalanced 'tolua_begin' directive in header file")
			end
		end
		c = c.."\n"
		c = string.gsub(c, "\n[^\n]*[Tt][Oo][Ll][Uu][Aa]_[hH][iI][Dd][eE][^\n]*\n", "\n");
		c = string.gsub(c, "#define[^%(\n]*%(.-\n", "\n")
		code = code .. c
	 _,e,c,t = strfind(s, "\n([^\n]-)[Tt][Oo][Ll][Uu][Aa]_([^%s]*)[^\n]*\n",e)
	end
	return code
end
