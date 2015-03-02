function usat = actSaturate(u, params)
    if u > params.uSat
        usat = params.uSat;
    elseif u < -params.uSat
        usat = -params.uSat;
    else
        usat = u;
    end
    