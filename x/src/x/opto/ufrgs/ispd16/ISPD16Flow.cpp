#include "ISPD16Flow.h"

#include "rsyn/io/Writer.h"
#include "rsyn/util/AsciiProgressBar.h"
#include "rsyn/util/Stepwatch.h"
#include "x/infra/iccad15/Infrastructure.h"


namespace ICCAD15 {

bool ISPD16Flow::run(Rsyn::Engine engine, const Rsyn::Json &params) {
	this->engine = engine;
	this->infra = engine.getService("ufrgs.ispd16.infra");
	this->timer = engine.getService("rsyn.timer");
	this->writer = engine.getService("rsyn.writer");
	this->design = engine.getDesign();
	this->module = design.getTopModule();
	
	runFlow();
	
	return true;
} // end method

// -----------------------------------------------------------------------------

void ISPD16Flow::runFlow() {
	Stepwatch watch("Flow: ISPD 2016");
	infra->legalize();

	// Initialization.
	timer->updateTimingIncremental();
	infra->updateQualityScore();
	infra->updateTDPSolution(true);

	// Early optimizations.
	engine.runProcess("ufrgs.earlyOpto");

	const bool earlyZeroed 
			= FloatingPoint::approximatelyZero(timer->getTns(Rsyn::EARLY));

	// Late optimizations.
	engine.runProcess("ufrgs.clusteredMove");
	infra->statePush("clustered-move");
	
	for (int i = 0; i < 10; i++) {
		bool keepGoing0 = false;
		bool keepGoing1 = false;
		bool keepGoing2 = false;
		
		engine.runProcess("ufrgs.balancing", {{"type", "buffer"}});
		timer->updateTimingIncremental();
		infra->updateQualityScore();
		infra->reportSummary("buffer-balancing");
		keepGoing0 = infra->updateTDPSolution();		
		infra->statePush("buffer-balancing");
		
		engine.runProcess("ufrgs.balancing", {{"type", "cell-steiner"}});
		timer->updateTimingIncremental();
		infra->updateQualityScore();
		infra->reportSummary("cell-balancing");
		keepGoing1 = infra->updateTDPSolution();				
		infra->statePush("cell-balancing");
		
		engine.runProcess("ufrgs.loadOpto");
		timer->updateTimingIncremental();
		infra->updateQualityScore();
		infra->reportSummary("load-opto");
		keepGoing2 = infra->updateTDPSolution();
		infra->statePush("load-opto");

		if (!keepGoing0 && !keepGoing1 && !keepGoing2)
			break;
	} // end for
	
	// Run extra-load reduction
	do {
		engine.runProcess("ufrgs.loadOpto");
		timer->updateTimingIncremental();
		infra->updateQualityScore();
		infra->reportSummary("load-opto-extra");
	} while (infra->updateTDPSolution(false, 0.005)); // 0.5%
	infra->statePush("load-opto");
	
	// ABU fix.
	engine.runProcess("ufrgs.abuReduction");
	infra->statePush("abu-fix");
	
	// Fix early violations.
	if (earlyZeroed && FloatingPoint::notApproximatelyZero(timer->getTns(Rsyn::EARLY))) {
		// If early was zeroed, but is not zero anymore, tries to optimize it
		// again.
		engine.runProcess("ufrgs.earlyOpto", {{"runOnlyEarlySpreadingIterative", true}});
		infra->statePush("iterative-spreading");
	} // end if
		
	// Workaround loop in rc trees.
	infra->runFluteLoopWorkaround();
	
	// Final timing update.
	timer->updateTimingIncremental();
	infra->updateQualityScore();
	infra->reportSummary("final");
	infra->reportFinalResult();
	// Save result.
	writer->writeDEF();
	writer->writeOPS( design.getName() + "-cada085.ops" );
} // end method

} // end namescape
