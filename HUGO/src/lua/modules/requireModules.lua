
function readModules()
    -- The name of the global table is what has been set in the sendArray method of the LuaBinder
    local modules = Modules
    Modules = nil

    for _,file in ipairs(modules) do
        require(file)
    end
end