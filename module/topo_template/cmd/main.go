package main

import (
	"flag"
	"log"
	"topo_template/topo"
	"topo_template/utils"
)

var (
	outPutFile = flag.String("f", "./topo_tmp.json", "default file for store topo")
)

func main() {
	flag.Parse()
	fsTopo, err := topo.NewTopo(utils.MgmtNodeType, "bigfs")
	if err != nil {
		log.Fatal(err)
	}
	if err = fsTopo.Dump(*outPutFile); err != nil {
		log.Fatal(err)
	}
}
