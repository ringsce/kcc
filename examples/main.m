// test.m
#import <Foundation/Foundation.h>
 
@interface Calculator : NSObject
@property (nonatomic, strong) NSString *name;
- (int)add:(int)a to:(int)b;
@end
 
@implementation Calculator
@synthesize name;
 
- (int)add:(int)a to:(int)b {
    return a + b;
}
@end
 
int main() {
    Calculator *calc = [[Calculator alloc] init];
    calc.name = @"MyCalculator";
    
    int result = [calc add:5 to:3];
    
    if (result == 8) {
        return 0;  // Success
    }
    return 1;
}
