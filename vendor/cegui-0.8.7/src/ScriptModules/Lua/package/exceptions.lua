exceptionDefs["CEGUI::Exception"]           = {}
exceptionDefs["CEGUI::Exception"]["var"]    = "&e"
exceptionDefs["CEGUI::Exception"]["c_str"]  = "e.getMessage().c_str()"

exceptionDefs["CEGUI::AlreadyExistsException"]  = exceptionDefs["CEGUI::Exception"]
exceptionDefs["CEGUI::InvalidRequestException"] = exceptionDefs["CEGUI::Exception"]
exceptionDefs["CEGUI::UnknownObjectException"]  = exceptionDefs["CEGUI::Exception"]
