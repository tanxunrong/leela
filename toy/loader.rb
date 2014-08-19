
begin

	class Leela
		def self.putCtx
			puts CTX
		end
	end

	Leela.putCtx()
	puts Leela.genid()
	#Leela.abort()
	if Leela.query(".logger") == nil
		puts "query nil"
	end
	puts Leela::PROTO_TYPE
	Leela.callback(:dispatch_msg)

rescue Exception => detail
	puts detail
end
