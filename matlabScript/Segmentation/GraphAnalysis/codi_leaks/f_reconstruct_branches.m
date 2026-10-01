

function [v1, v2] = f_reconstruct_branches(pts1, pts2, v_mask)
%%----------------------------------------------------------------------------------------
% pts1: indexos de voxels pertanyents al esquelet 1 
% pts2: indexos de voxels pertanyents al esquelet 2
% v_mask: mascara del volum a reconstruir
% conn: connectivitat
% retorna:
% v1: volum de les branques sanes
% v2: volum dels leaks a eliminar
%Exemple:
% load('AgnesData.mat')
% pts1= PtsBranca; %punts de l'esquelet de la branca 'sana'
% pts2= PtsLeak; %punts de l'esquelet dl leak
% v_mask = double(DistalBranch); %mascara de tota la branca
% [v1, v2] = f_reconstruct_branches(pts1,pts2, v_mask);
%%----------------------------------------------------------------------------------------

%creem dos volums amb els voxels inicials dels esquelets a 1
n_pix_vol = size(v_mask,1)*size(v_mask,2)*size(v_mask,3);
v1 = double(zeros(size(v_mask)));
v2 = double( zeros(size(v_mask)));
v1(pts1) = 1;
v2(pts2) = 1;
v1 =double(v1.*v_mask);
v2 = double(v2.*v_mask);

%fem un bucle en que els fem creixer morfològicament fins que omplen tota
%la mascara o fins que el creixement no canvia
se = strel('sphere',1);
fi = false;
%n=0;
while (fi==false)
   
  
    %comptem quants pixels falten per omplir
    v_ple = max(~v_mask, max(v1, v2));
    n_pix_falta = n_pix_vol-sum(v_ple(:));
    clear('v_ple');
    
    %si ja està ple sortim
    if (n_pix_falta==0)
        fi = true;
        disp('fi: esta tot ple');
    
    %si no esta ple fem un dilate de v1 i v2 
    else
        v1 = imdilate(v1,se);
        v1 = v1.*v_mask;
        v1 = v1.*(~ v2);
              
       v2 = imdilate(v2,se);       
        v2 = v2.*v_mask;
        v2 = v2.*(~ v1);
         
        %mirem quants pixels queden per omplir
        v_ple_new = max(~v_mask, max(v1, v2));
        n_pix_falta_new = n_pix_vol-sum(v_ple_new(:));
        clear('v_ple_new');
         
        %si no hem icorporat nous voxels sortim
        if (n_pix_falta_new==n_pix_falta)
            fi = true;
         %   disp('fi: expansió no incorpora nous voxels');
        end
    end

%       n=n+1
%       n_pix_falta
%       n_pix_falta_new
end



