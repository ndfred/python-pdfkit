#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

int generatePages(const char *documentPath) {
	int error = 0;
	CFStringRef path = CFStringCreateWithCString(
		NULL, documentPath, kCFStringEncodingUTF8
	);
	CFURLRef url = CFURLCreateWithFileSystemPath(
		NULL, path, kCFURLPOSIXPathStyle, 0
	);
	CGPDFDocumentRef document = CGPDFDocumentCreateWithURL(url);

	if (document != NULL) {
		size_t lastPageIndex = CGPDFDocumentGetNumberOfPages(document) + 1;
		CFStringRef pdfPath = CFSTR("document.pdf");
		CFURLRef pdfUrl = CFURLCreateWithFileSystemPath(
			NULL, pdfPath, kCFURLPOSIXPathStyle, 0
		);
		CGContextRef context = CGPDFContextCreateWithURL(pdfUrl, NULL, NULL);

		printf("Compressing PDF\n");
		for(size_t pageIndex = 1; pageIndex < lastPageIndex; ++pageIndex) {
			CGPDFPageRef page = CGPDFDocumentGetPage(document, pageIndex);
			const CGRect cropBox = CGPDFPageGetBoxRect(page, kCGPDFCropBox);

			printf("Adding page %lu\n", pageIndex);
			CGContextBeginPage(context, &cropBox);
			CGContextDrawPDFPage(context, page);
			CGContextEndPage(context);
		}

		CGContextRelease(context);
		if (pdfUrl != NULL) CFRelease(pdfUrl);
		if (pdfPath != NULL) CFRelease(pdfPath);
		CGPDFDocumentRelease(document);
	} else {
		fprintf(stderr, "Cannot open PDF document\n");
		error = 1;
	}

	if(url != NULL) CFRelease(url);
	if(path != NULL) CFRelease(path);

	return error;
}

int main(int argc, char *argv[]) {
	int error = 0;

	if (argc != 2) {
		fprintf(stderr, "Please specify a PDF file name\n");
		error = 1;
	} else {
		const char *documentPath = argv[1];
		if (generatePages(documentPath) != 0) {
			fprintf(stderr, "Cannot generate pages\n");
			error = 2;
		}
	}

	return error;
}
