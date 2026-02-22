local use64 = false
function maetro64()
    use64 = true
end

local _project = project
function project(name)
    _project(name)

    if name ~= "maetro32" and name ~= "maetro64" then
        links { use64 and "maetro64" or "maetro32", "kernel32.lib"}
        linkoptions { "/NODEFAULTLIB:kernel32" }
        use64 = false
    end
end
