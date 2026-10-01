%==========================================================================
% FUNCTION NAME :  [Vol]= PrincipalConnComp(Vol,Conn,NComp);
%--------------------------------------------------------------------------
% DESCRIPTION :
% Retorna les NComp components connexes mes grans (etiquetades) del volum Vol
%--------------------------------------------------------------------------
% INPUTS :
%  1> Vol: Volumen (mascara)
%  2> Conn: Conectivitat per definir les components connexes (mateixa que
%  bwlabeln)
%  3> NComp: Numero de components connexes
%
%--------------------------------------------------------------------------
% OUTPUTS : Vol: Comp. connexes etiquetades
%
%--------------------------------------------------------------------------
% EXTERNAL FUNCTIONS :
%--------------------------------------------------------------------------
% RELATED BIBLIOGRAPHY :
%--------------------------------------------------------------------------
% CREATION DATE : 10/02/2012
%--------------------------------------------------------------------------
% LAST MODIFICATION :
%--------------------------------------------------------------------------
% AUTHOR : Debora Gil, Agnes Borras
%--------------------------------------------------------------------------
% % USAGE EXAMPLES : La component connexa principal (mes gran)
%
% [Vol]= PrincipalConnComp(Vol,6,1);
%==========================================================================

function [Vol]= PrincipalConnComp(Vol,Conn,NComp)

[Lab]=bwlabeln(double(Vol),Conn);
p=regionprops(Lab);
[val,ValLab]=sort([p.Area],'descend');

Vol=0*Vol;
NComp=min(NComp,max(Lab(:)));

%Vol=ismember(Lab,ValLab(1:NComp));

if(length(ValLab))
    for k=1:NComp
        Vol=Vol+double(Lab==ValLab(k))*k;
    end
end