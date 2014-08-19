
begin

	Leela.callback()

	class Leela
		def self.putCtx
			puts @@CTX
		end
	end

	Leela.putCtx()

rescue Exception => detail
	puts detail
end
