
begin

	Leela.callback()

	class Leela
		attr_reader :CTX
		def self.putCtx
			puts @@CTX
		end
	end

	Leela.putCtx()

rescue Exception => detail
	puts detail
end
