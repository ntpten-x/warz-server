; Shim for PhysX 3.2.4 static libs (type_info::name(__type_info_node*)).
.686P
.MODEL FLAT
.DATA
shim_name DB 'type', 0
.CODE
PUBLIC ?name@type_info@@QBEPBDPAU__type_info_node@@@Z
?name@type_info@@QBEPBDPAU__type_info_node@@@Z PROC
    mov eax, OFFSET shim_name
    ret 4
?name@type_info@@QBEPBDPAU__type_info_node@@@Z ENDP
END
