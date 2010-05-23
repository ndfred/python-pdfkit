#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

int generateThumbnails(const char *documentPath) {
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
		const CGSize thumbnailSize = CGSizeMake(600.0, 800.0);
		const CGRect thumbnailRect = CGRectMake(
			0.0, 0.0, thumbnailSize.width, thumbnailSize.height
		);
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		CGContextRef context = CGBitmapContextCreate(
			NULL,
			thumbnailSize.width, thumbnailSize.height,
			8, thumbnailSize.width * 4,
			colorSpace, kCGImageAlphaNoneSkipLast
		);

		CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);

		for(size_t pageIndex = 1; pageIndex < lastPageIndex; ++pageIndex) {
			CGPDFPageRef page = CGPDFDocumentGetPage(document, pageIndex);
			CGImageRef image = NULL;
			CFStringRef thumbnailPath = CFStringCreateWithFormat(
				NULL, NULL, CFSTR("%lu.png"), pageIndex
			);
			CFURLRef thumbnailUrl = CFURLCreateWithFileSystemPath(
				NULL, thumbnailPath, kCFURLPOSIXPathStyle, 0
			);
			CGImageDestinationRef destination = CGImageDestinationCreateWithURL(
				thumbnailUrl, kUTTypePNG, 1, NULL
			);
			CGAffineTransform transform = CGPDFPageGetDrawingTransform(
				page, kCGPDFMediaBox, thumbnailRect, 0, 1
			);

			printf("Creating thumbnail for page %lu\n", pageIndex);
			CGContextFillRect(context, thumbnailRect);
			CGContextConcatCTM(context, transform);
			CGContextDrawPDFPage(context, page);
			CGContextConcatCTM(context, CGAffineTransformInvert(transform));
			image = CGBitmapContextCreateImage(context);
			CGImageDestinationAddImage(destination, image, NULL);
			CGImageDestinationFinalize(destination);

			if (destination != NULL) CFRelease(destination);
			if (image != NULL) CFRelease(image);
			if (thumbnailUrl != NULL) CFRelease(thumbnailUrl);
			if (thumbnailPath != NULL) CFRelease(thumbnailPath);
		}

		CGColorSpaceRelease(colorSpace);
		CGContextRelease(context);
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
		if (generateThumbnails(documentPath) != 0) {
			fprintf(stderr, "Cannot generate thumbnails\n");
			error = 2;
		}
	}

	return error;
}
