; ModuleID = 'simple_loop5_vs_3.c'
source_filename = "simple_loop5_vs_3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@mutex = dso_local global i32 0, align 4
@a = dso_local global i32 1, align 4
@b = dso_local global i32 2, align 4
@c = dso_local global i32 3, align 4
@temp = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %7, %0
  br label %2

2:                                                ; preds = %6, %1
  %3 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @a, align 4
  store i32 %8, i32* @temp, align 4
  %9 = load i32, i32* @b, align 4
  store i32 %9, i32* @a, align 4
  %10 = load i32, i32* @c, align 4
  store i32 %10, i32* @b, align 4
  %11 = load i32, i32* @temp, align 4
  store i32 %11, i32* @c, align 4
  store i32 0, i32* @mutex, align 4
  br label %1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr3() #0 {
  br label %1

1:                                                ; preds = %7, %0
  br label %2

2:                                                ; preds = %6, %1
  %3 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @a, align 4
  store i32 %8, i32* @temp, align 4
  %9 = load i32, i32* @b, align 4
  store i32 %9, i32* @a, align 4
  %10 = load i32, i32* @c, align 4
  store i32 %10, i32* @b, align 4
  %11 = load i32, i32* @temp, align 4
  store i32 %11, i32* @c, align 4
  store i32 0, i32* @mutex, align 4
  br label %1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %0, %12
  br label %2

2:                                                ; preds = %6, %1
  %3 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @a, align 4
  %9 = load i32, i32* @b, align 4
  %10 = icmp ne i32 %8, %9
  br i1 %10, label %12, label %11

11:                                               ; preds = %7
  call void @__ASSERT()
  br label %12

12:                                               ; preds = %11, %7
  store i32 0, i32* @mutex, align 4
  br label %1
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
