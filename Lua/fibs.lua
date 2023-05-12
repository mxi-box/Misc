--- Fibs

function fibs(n)
	if n == 0 then
		return 0
	elseif n == 1 or n == 2 then
		return 1
	else
		return fibs(n - 1) + fibs(n - 2)
	end
end

for n = 1, 40, 1 do
	print(n, " == ", fibs(n))
end