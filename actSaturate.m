function usat = actSaturate(u, satValue)
    if u > satValue
        usat = satValue;
    elseif u < -satValue
        usat = -satValue;
    else
        usat = u;
    end
    