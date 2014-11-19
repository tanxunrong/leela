
class Leela 

	PROTO_TYPE = [
	   	:PTYPE_TEXT => 0,
		:PTYPE_RESPONSE => 1,
		:PTYPE_MULTICAST => 2,
		:PTYPE_CLIENT => 3,
		:PTYPE_SYSTEM => 4,
		:PTYPE_HARBOR => 5,
		:PTYPE_SOCKET => 6,
		:PTYPE_ERROR => 7,
		:PTYPE_QUEUE => 8,
		:PTYPE_DEBUG => 9,
		:PTYPE_LUA => 10, 
		:PTYPE_SNAX => 11]

	def dispatch_msg(prototype,msg,sz,session,source,*rest)
		if prototype == PROTO_TYPE[:PTYPE_RESPONSE]
			"type response"
		else
			"other type"
		end
	end

	## skynet commands 

	def self.abort()
		self.command("ABORT")
	end

	# find global service like .logger .harbor 's handle with :0x format
	def self.query(service_name)
		self.command("QUERY",service_name)
	end

	def self.now
		self.command("NOW").to_i
	end

	def self.starttime
		self.command("STARTTIME").to_i
	end


end

