# Architecture

`SoundWorkflow` and `ImageBuildWorkflow` are GRASP controllers for separate use cases. They own state and sequencing while `ISoundDevice`, `IImageStore` and `IWorkshopStore` protect them from platform changes. The image builder and preview decoder are information experts for their data formats.

`XboxWorkshopStore` adapts static native file services to the core port. `OperationRunner` is an active object: it runs a use case on a worker and publishes a synchronized status snapshot. A single operation-kind enum replaces independent flags which could express contradictory modes.

`PreviewPlayer` owns the XAudio2 engine, voices and sample-buffer lifetime. PCM byte order is converted only at this device boundary. `CatalogueModel` owns selection and pagination; `CatalogueList` presents it using fixed native XUI elements. `StudioScene` coordinates navigation and localized presentation.

Hardware transfer timing and the ISD image format were preserved during this cleanup. UI controllers can be split further as more screens are added; no generic framework is required for the current number of use cases.
