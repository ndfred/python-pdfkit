#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

size_t countDocumentPages(const char *documentPath) {
	size_t numberOfPages = 0;
	CFStringRef path = CFStringCreateWithCString(
		NULL,
		documentPath,
		kCFStringEncodingUTF8
	);
	CFURLRef url = CFURLCreateWithFileSystemPath(
		NULL,
		path,
		kCFURLPOSIXPathStyle,
		0
	);
	CGPDFDocumentRef document = CGPDFDocumentCreateWithURL(url);

	if (document != NULL) {
		numberOfPages = CGPDFDocumentGetNumberOfPages(document);
		CGPDFDocumentRelease(document);
	} else {
		fprintf(stderr, "Cannot open PDF document\n");
	}

	if(url != NULL) CFRelease(url);
	if(path != NULL) CFRelease(path);

	return numberOfPages;
}

int main(int argc, char *argv[]) {
	int error = 0;

	if (argc != 2) {
		fprintf(stderr, "Please specify a PDF file name\n");
		error = 1;
	} else {
		const char *documentPath = argv[1];
		size_t numberOfPages = countDocumentPages(documentPath);

		if (numberOfPages != 0) {
			printf("Document %s has %lu pages\n", documentPath, numberOfPages);
		} else {
			fprintf(stderr, "Cannot get PDF document number of pages\n");
			error = 2;
		}
	}

	return error;
}
