%Print skel final
function G = FlipSkel(G,vol,ROI)
G.weUniformX={};
G.weUniformY={};
G.weUniformZ={};

%girem arestes
for i=1:size(G.e,1)
    for j=1:size(G.e,2)
     if G.e(i,j)>0
        [x,y,z] = ind2sub(size(vol),G.we{i,j}(:));
        %fem interpolació uniforme dels punts d'una branca
        [xi,yi,zi] = ArcLengthBranchParametrization(x',y',z');
        %Girem la Z dels punts uniformes
        zi = FlipZDim(vol,zi');
        %Girem X,Y els nodes i desplacem X,Y,Z a la ROI
        v_tempx= xi;
        v_tempy= yi;
        xi = v_tempy;
        yi = v_tempx;
        xi=xi+ROI.j(1)-1;
        yi=yi+ROI.i(1)-1;
        zi=zi+ROI.k(1)-1;
        %Girem Z dels nodes
        G.weUniformX{i,j}(:)=xi;
        G.weUniformY{i,j}(:)=yi;
        G.weUniformZ{i,j}(:)=zi;
     end
    end
end
%girem fulles --> no esta fet



end