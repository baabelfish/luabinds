fail = false

if (v_size() ~= 3 or v_empty() ~= false) then fail = true end
if (v_at(1) ~= 2 or v_at(2) ~= 3) then fail = true end

v_set(1, 11)
if (v_at(1) ~= 11) then fail = true end

v_clear()
if (v_size() ~= 0 or v_empty() ~= true) then fail = true end

v_push(42)
if (v_size() ~= 1 or v_empty() ~= false or v_at(0) ~= 42) then fail = true end
