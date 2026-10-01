function [CurvL]=BranchLength(x,y,z)

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
S=t{indMx}(indP);

CurvL=sum(sqrt(sum(diff(Pt').^2,2)).*diff(S'));

