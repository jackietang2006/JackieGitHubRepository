//polymorphism implementation via interface & handler inside type struct {}

package main

import (
	"fmt"
	"os"
	"strconv"
)

type handlerFunc interface {
        executeHandler() int
}

type App1Fsm struct {
        *BaseFsm
}

type App2Fsm struct {
        *BaseFsm
}

type BaseFsm struct {
	counter int
	handler handlerFunc //delegating to self for the polymorphic executeHandler()
}

//overriding method
func (fsm *App1Fsm) executeHandler() int {
	fsm.counter ++
	return fsm.counter
}

//overriding method
func (fsm *App2Fsm) executeHandler() int {
	fsm.counter ++
	return fsm.counter
}

//base method
func (base *BaseFsm) executeHandler() {
	fmt.Printf("invoking BaseFsm::executeHandler()\n")
}

func newBaseFsm(handler handlerFunc) *BaseFsm {
	base := &BaseFsm{}
	base.handler = handler
	return base
}

func newApp1Fsm() *App1Fsm {
	fsm := &App1Fsm{}
	fsm.BaseFsm = newBaseFsm(fsm)
	return fsm
}

func newApp2Fsm() *App2Fsm {
	fsm := &App2Fsm{}
	fsm.BaseFsm = newBaseFsm(fsm)
	return fsm
}

func main() {
	argc := len(os.Args)
	argv := os.Args

	if argc < 2 {
		os.Exit(1)
	}
	numIterations, _ := strconv.Atoi(argv[1])

	var app handlerFunc
	var executeCounter int
	app = newApp1Fsm()
	for i := 0; i < numIterations; i++ {
		executeCounter = app.executeHandler()
	}
	fmt.Printf("App1Fsm::app.executeHandler() %d times\n", executeCounter)

	app = newApp2Fsm()
	for i := 0; i < numIterations; i++ {
		executeCounter = app.executeHandler()
	}
	fmt.Printf("App2Fsm::app.executeHandler() %d times\n", executeCounter)
}
