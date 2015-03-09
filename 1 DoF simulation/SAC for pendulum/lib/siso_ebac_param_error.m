function err = siso_ebac_param_error(theta,thetaP,xi,xiP,psi,psiP)
%SISO_EBAC_PARAM_ERROR Calculate parameter error
%
% ERR = SISO_EBAC_PARAM_ERROR(THETA, THETAP, XI, XIP, PSI, PSIP)
% calculates the parameter errors for the SISO EB-PBC AC algorithm
%
% INPUTS:   THETA, THETAP: Critic parameter vectors
%
%           XI, XIP: Energy-shaping parameter vectors
%
%           PSI, PSIP: Damping injection parameter vectors
%

err = [sqrt((thetaP-theta)'*(thetaP-theta)); sqrt((xiP-xi)'*(xiP-xi)); sqrt((psiP-psi)'*(psiP-psi))];