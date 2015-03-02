function action = actor(state, params) 
%     phi = transpose(params.phi)    
%     rbfactor = transpose(rbf(state,params))
    action = params.phi'*rbf(state,params)     % calculate action
    
    
    

    