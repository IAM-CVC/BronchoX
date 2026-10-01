function [CITh,ThAct]=Update_Th(BranchComplexity,CITh,ThAct,CompTh)

if(BranchComplexity<=CompTh)
   CITh(2)=ThAct;

elseif(BranchComplexity>CompTh)
    CITh(1)=ThAct;
  
end

ThAct=mean(CITh);