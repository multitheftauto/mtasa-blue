-- tolua: namespace class
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 2003
-- $Id$

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications.


-- Namespace class
-- Represents a namesapce definition.
-- Stores the following fields:
--    name = class name
--    {i}  = list of members
classNamespace = {
 classtype = 'namespace',
 name = '',
}
classNamespace.__index = classNamespace
setmetatable(classNamespace,classModule)

-- Print method
function classNamespace:print (ident,close)
 print(ident.."Namespace{")
 print(ident.." name = '"..self.name.."',")
 local i=1
 while self[i] do
  self[i]:print(ident.." ",",")
  i = i+1
 end
 print(ident.."}"..close)
end


--- 
-- LuaDoc Patch
-- outputs an empty(without documentation) LuaDoc interface 
-- by klapeto (http://cegui.org.uk/forum/viewtopic.php?f=7&t=6784)
function classNamespace:output_luadoc()

	output('---------------------------------------------------------------------------------\n')
	if ( not self.name and not self.name~='') then
		output('-- @field [parent=#'..cleanseType(self.parent.name)..'] #'..cleanseType(self.name)..' '..cleanseType(self.name)..'\n')
	else
		local name = cleanseType(self.name)
		output('---------------------------------------------------------------------------------\n')
		output('-- @module '..name..'\n')
		output('\n')
		output('---\n')
		output('-- @field [parent=#global] #'..name..' '..name..'\n')
	end
	
	output('\n')
	
	output('---------------------------------------------------------------------------------\n')
	output('-- '..self.name..'\n')
	output('-- @type '..cleanseType(self.name)..'\n')
	
	output('\n')
		local i=1
	while self[i] do
		self[i]:output_luadoc()
		i = i+1
	end
end

-- Internal constructor
function _Namespace (t)
 setmetatable(t,classNamespace)
 append(t)
 return t
end

-- Constructor
-- Expects the name and the body of the namespace.
function Namespace (n,b)
 local c = _Namespace(_Container{name=n})
 push(c)
 c:parse(strsub(b,2,strlen(b)-1)) -- eliminate braces
 pop()
end

