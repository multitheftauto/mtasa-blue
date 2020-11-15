-- tolua: define class
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 1998
-- $Id$

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications.


-- Define class
-- Represents a numeric const definition
-- The following filds are stored:
--   name = constant name
classDefine = {
 name = '',
}
classDefine.__index = classDefine
setmetatable(classDefine,classFeature)

-- register define
function classDefine:register (pre)
	if not self:check_public_access() then
		return
	end

 pre = pre or ''
 output(pre..'tolua_constant(tolua_S,"'..self.lname..'",'..self.name..');')
end

--- 
-- LuaDoc Patch
-- outputs an empty(without documentation) LuaDoc interface 
-- by klapeto (http://cegui.org.uk/forum/viewtopic.php?f=7&t=6784)
function classDefine:output_luadoc()
	if not self:check_public_access() then
		return
	end
	if (self.parent~=nil) then
		output('---\n')
		output('-- @field [parent=#'..cleanseType(self.parent.name)..'] #string '..self.lname..'\n')
	else
		output('---\n')
		output('-- @field [parent=#global] #string '..self.lname..'\n')
	end
end

-- Print method
function classDefine:print (ident,close)
 print(ident.."Define{")
 print(ident.." name = '"..self.name.."',")
 print(ident.." lname = '"..self.lname.."',")
 print(ident.."}"..close)
end


-- Internal constructor
function _Define (t)
 setmetatable(t,classDefine)
 t:buildnames()

 if t.name == '' then
  error("#invalid define")
 end

 append(t)
 return t
end

-- Constructor
-- Expects a string representing the constant name
function Define (n)
 return _Define{
  name = n
 }
end


