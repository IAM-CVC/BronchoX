function []=ArcLengthBranchParametrization(x,y,z,varargin)

% Given (x,y,z) a list of discrete points, try this code to obtain a
% sampling uniformily spaced


%NPts=200;
t{1}=x;
t{2}=y;
t{3}=z;

%Compute curve paramenter
for k=1:length(t)
    [~,ind{k}]=unique(t{k});
    L(k)=length(ind{k});
end

[~,indMx]=max(L);
indP=ind{indMx};

Pt=[x(indP);y(indP);z(indP)];
Param=t{indMx}(indP);

%Parametro Arco 
S=[0 cumsum(sqrt(sum(diff(Pt').^2,2)).*diff(Param'))'];

if(1-isempty(varargin))
    NPts=varargin{1};
else
    NPts=length(S);
end

%Reparametritzación con spacing uniforme
SI=0:S(end)/NPts:S(end);
xS=interp1(S,Pt(1,:),SI);
yS=interp1(S,Pt(2,:),SI);
zS=interp1(S,Pt(3,:),SI);
