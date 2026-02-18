local _project = project
function project(name)
    _project(name)

    if name ~= "maetro32" then
        links { "maetro32", "kernel32.lib"}
        linkoptions { "/NODEFAULTLIB:kernel32" }
    end
end
