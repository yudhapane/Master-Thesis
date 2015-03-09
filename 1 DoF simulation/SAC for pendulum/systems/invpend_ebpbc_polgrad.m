function [polgradES polgradDI] = invpend_ebpbc_polgrad(phiA,dphiA,dphiCdx,theta,g,dHddx,ducdu,dk,Delta_u,par)
%INVPEND_EBPBC_POLGRAD Computes EB-PBC policy gradient
%
% [polgradES polgradDI] =
% invpend_ebpbc_polgrad(phiA,dphiA,J,R,g,dHddx,dHdx) computes the 
% gradient
% 
%   [du/dxi du/dpsi]
%
% of the Energy-Balancing PBC control policy
% 
%       u = (g^T(x)*g(x))^(-1)*g^T(x)*(F*(dHddx - dHdx)) - Kd*g^T(x)*dHddx
%
% in which dHddx and Kd are parameterized according to
%
% dHddx = [xi'*dphiA.ES; dHdx(2)]
%
% Kd    = psi'*phiA.DI
%
% and subject to input saturation.
% 
% INPUTS:   phiA: Actor feature values
%           
%           dphiA: feature value gradients.
%
%           dphiCdx: critic feature value gradients
%
%           theta: critic parameter vector
%
%           g: system input matrix. See also invpend_eom
%
%           dHddx: gradient of desired Hamiltonian
%
%           ducdu: partial derivative of u wrt saturation function. See
%           also sat
%
%           dk: temporal difference. See also siso_ebac
%
%           Delta_u: exploratory action (if present)
%
%           par: structure containing simulation parameters
%
% OUTPUTS: POLGRADES: Policy gradient of the energy-shaping part
%
%          POLGRADDI: Policy gradient of damping-injection part
%

dpidxi      = ducdu*pinv(g(2))*-dphiA.ES;
dpidpsi     = ducdu*-phiA.DI*g'*dHddx;

if ~isfield(par,'polgrad')
    polgradES = Delta_u*dk*dpidxi;
    polgradDI = Delta_u*dk*dpidpsi;
elseif strcmp(par.polgrad,'model')
    polgradES = (theta'*dphiCdx)*g*dpidxi;
    polgradDI = (theta'*dphiCdx)*g*dpidpsi;
end
    
    