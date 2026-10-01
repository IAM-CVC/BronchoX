function [CITh,ThAct]=Update_Th_05_12_2017(BranchComplexity,CITh,ThAct,CompTh)

NTh=length(BranchComplexity);
for k=1:NTh
    
    if(BranchComplexity(k)<=CompTh)
        CITh(k,2)=ThAct(k);
        
    elseif(BranchComplexity(k)>CompTh)
        CITh(k,1)=ThAct(k);
        
    end
    
    ThAct(k)=mean(CITh(k,:));
end