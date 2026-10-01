
function [node_eliminat, TG, is_root] = f_elimina_node_minima_energia(TG, LoopNodes)


%trobar  valor energia loop nodes
%agafem el node amb menor energia
eln = TG.wv(LoopNodes,2); 
[val,pos] = sort(eln);  
lp_min = LoopNodes(pos(1));
 
 %li eliminem els edges
TG.e(lp_min,:) = 0;
TG.e(:,lp_min) = 0;
node_eliminat = lp_min;
    
%si el de menys energia no és el root tornem un boolea
if (lp_min~= TG.rootNode)
   is_root = false;  
else
     is_root = true;
end
