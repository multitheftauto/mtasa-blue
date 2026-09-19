function countArgs(...)
    return select("#", ...)
end

function deepReturn()
    local root = {}
    local current = root
    for i = 2, 65 do
        current.nested = {}
        current = current.nested
    end
    return "before", root, "after"
end

function checkReferences(value)
    return value.self == value, value.a == value.b
end

function echo(...)
    return ...
end
