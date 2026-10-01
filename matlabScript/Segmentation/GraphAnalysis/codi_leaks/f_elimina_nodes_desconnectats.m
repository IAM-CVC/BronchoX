

function [nodes_desconnectats, TG] = f_elimina_nodes_desconnectats(TG)

%fer les components connexes
[indComp, nnodes] = components(sparse(TG.e));
lab_root = indComp(TG.rootNode);
nodes_desconnectats = find(indComp~=lab_root);

%eliminem les connexions
if (~isempty(nodes_desconnectats))
    TG.e(nodes_desconnectats(:),:) = 0;
    TG.e(:,nodes_desconnectats(:)) = 0;
end

 




