# kirinfs mgs

### mgs 功能概述

![](./doc/MGS.png)

- kirifs需要存储整个文件系统的配置信息，比如文件系统名称、配置参数

- 客户端挂载后的mdc/mgc/client的整体拓扑图，方便mgs管理工具针对这些组件进行动态的更改

- mgs的客户端管理工具，基于整体的topo图来操作维护各个组件的runtime